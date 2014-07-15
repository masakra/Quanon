#include "processor.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickImageProvider>

#include <QAudioFormat>
#include <QAudioRecorder>
#include <QAudioInput>
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include <QtSql>
#include <QtXml>

#include "Riff.h"

#include <QDebug>

Processor::Processor(QObject *parent) :
    QObject(parent), m_audio_in( 0 ), m_nam( new QNetworkAccessManager( this ) )
{
    QSqlDatabase sqlite = QSqlDatabase::addDatabase("QSQLITE");
    sqlite.setDatabaseName("quanon");

    if ( sqlite.open() ) {
        // rowid will be used for unique row id
        sqlite.exec("CREATE TABLE IF NOT EXISTS message (value text)");
    } else
        qCritical() << "Ошибка открытия базы данных";

}

void
Processor::processImage( const QString & path )
{
    QUrl imageUrl( path );

    QQmlEngine * engine = QQmlEngine::contextForObject( this )->engine();
    QQmlImageProviderBase * imageProviderBase = engine->imageProvider( imageUrl.host() );
    QQuickImageProvider * imageProvider = static_cast< QQuickImageProvider * >( imageProviderBase );

    QSize imageSize;
    QString imageId = imageUrl.path().remove( 0, 1 );
    m_image = imageProvider->requestImage( imageId, &imageSize, imageSize );

    if ( ! m_image.isNull() )
        qDebug() << m_image.size();
    else
        qDebug() << "imageIsNull";

    audioCapture();
}

void
Processor::audioCapture()
{
    m_format.setSampleRate( 8000 );
    m_format.setSampleSize( 16 );
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder( QAudioFormat::LittleEndian );
    m_format.setSampleType( QAudioFormat::UnSignedInt );

    m_audio_buffer.open( QIODevice::WriteOnly | QIODevice::Truncate );

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();

    if ( ! info.isFormatSupported( m_format ) ) {
        qWarning() << "default format do not supported, use nearest";
        m_format = info.nearestFormat( m_format );
    }

    m_audio_in = new QAudioInput( m_format, this );
    m_audio_in->start( &m_audio_buffer );
}

void Processor::send( const QString & server )
{
    qDebug() << "send( server )";
    if ( server.isEmpty() )
        return;

    m_server = server;

    QSqlQuery q;
    q.prepare("SELECT "
                 "rowid, "
                 "value "
              "FROM "
                 "message "
              "ORDER BY "
                 "rowid "
              "LIMIT "
                 "1");

    if ( q.exec() ) {
        if ( q.first() ) {
            m_rowid = q.value( 0 ).toInt();
         // отправить
         emit message( QString("Sending message %1").arg( m_rowid ) );

         QUrl url( server );
         url.setPort(20001);

         QNetworkRequest request( url );
         request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant("application/xml") );

         emit sendBegin();

         m_reply = m_nam->post( request, q.value( 1 ).toByteArray() );

         connect( m_reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
                 SLOT( replyError( QNetworkReply::NetworkError ) ) );
         connect( m_reply, SIGNAL( sslErrors( QList< QSslError> ) ),
                 SLOT( replySslErrors( QList< QSslError > ) ) );
         connect( m_reply, SIGNAL( finished() ), SLOT( replyFinished() ) );
       }
   }
}

void
Processor::stopAudioRecord()
{
    if ( m_audio_in ) {
        m_audio_in->stop();
        delete m_audio_in;
        m_audio_in = 0;
    }
}

void
Processor::save( const QString & author )
{
    stopAudioRecord();

    QBuffer buf;
    buf.open( QIODevice::WriteOnly );

    if ( ! m_image.save( &buf, "JPG" ) ) {
        qWarning() << "Error image to buffer writing";
    }

    QDomDocument doc;

    QDomElement root = doc.createElement("Elements");
    doc.appendChild( root );

    // Fields
    QDomElement element = doc.createElement("Element");
    doc.appendChild( element );

    QDomElement fields = doc.createElement("Fields");
    element.setAttribute("Alias","quanon");
    element.appendChild( fields );

    QDomElement fieldAuthor = doc.createElement("Field");
    fieldAuthor.setAttribute("Name", "Author");
    fieldAuthor.appendChild( doc.createTextNode( author ) );
    fields.appendChild( fieldAuthor );

    element.appendChild( fields );

    // Files
    QDomElement files = doc.createElement("Files");

    QDomElement fileAudio = doc.createElement("File");
    fileAudio.setAttribute("Name", "audio.wav");
    fileAudio.setAttribute("sampleRate", m_format.sampleRate() );
    fileAudio.setAttribute("sampleSize", m_format.sampleSize() );
    fileAudio.setAttribute("bypeOrder", m_format.byteOrder() );
    fileAudio.setAttribute("sampleType", m_format.sampleType() );
    fileAudio.appendChild( doc.createTextNode( Riff( m_format ).rawData( m_audio_buffer ).toBase64() ) );
    files.appendChild( fileAudio );

    QDomElement fileImage = doc.createElement("File");
    fileImage.setAttribute("Name", "image.jpg");
    fileImage.appendChild( doc.createTextNode( buf.buffer().toBase64() ) );
    files.appendChild( fileImage );

    element.appendChild( files );

    root.appendChild( element );

    QDomNode pi = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    doc.insertBefore( pi, doc.firstChild() );

    qDebug() << doc.toString();

    // сохранить
    const QString xmlText = doc.toString();

    if ( xmlText.isEmpty() )
        return;

    QSqlQuery q;

    q.prepare("INSERT INTO message ( value ) VALUES ( :value )");
    q.bindValue(":value", xmlText );

    if ( ! q.exec() )
        qCritical() << q.lastError().text();

}

void
Processor::replyError( QNetworkReply::NetworkError error )
{
    qCritical() << "NetworkError: " << error;
}

void
Processor::replySslErrors( QList< QSslError > errorList )
{
    int count = 0;
    foreach( QSslError sslError, errorList ) {
        qCritical() << ++count << " SSL error: " << sslError.errorString();
    }
}

void
Processor::replyFinished()
{
    qDebug() << "replyFinished" << m_reply->errorString();
    const QNetworkReply::NetworkError ne = m_reply->error();
    m_reply->deleteLater();
    emit sendEnd();
    emit message("Message delivered: " + m_reply->errorString() );

    if ( ne != 0 )
        return;

    // удаление сообщения из SQLite базы данных

    QSqlQuery q;
    q.prepare("DELETE FROM message WHERE rowid = :rowid");
    q.bindValue(":rowid", m_rowid );
    if ( q.exec() )
        send( m_server );
    else
        qDebug() << q.lastError().text();

}
