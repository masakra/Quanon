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

#include <QtXml>

#include "Riff.h"

#include <QDebug>

Processor::Processor(QObject *parent) :
    QObject(parent), m_audio_in( 0 ), m_nam( new QNetworkAccessManager( this ) )
{
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
Processor::send( const QString & author, const QString & server )
{
    stopAudioRecord();

//    QJsonObject json;

//    QByteArray img;
    QBuffer buf;//( &img );
    buf.open( QIODevice::WriteOnly );

    if ( ! m_image.save( &buf, "JPG" ) ) {
        qWarning() << "Error image to buffer writing";
        //return; TODO uncomennt this line
    }

    /*
    json.insert("img", QJsonValue::fromVariant( buf.buffer() ) );

    json.insert("audio", QJsonValue::fromVariant( m_audio_buffer.buffer() ) );

    // JSON готов к отправке
    // или сделать QJsonDocument ?

    QJsonDocument jsonDoc( json );

    qDebug() << jsonDoc.toJson();

    QNetworkRequest request( QUrl("http://192.168.0.160/quanon.php") );
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );

    //nam->post( QNetworkRequest( QUrl("http://192.168.0.160/quanon.php") ),
    nam->post( request, jsonDoc.toJson() );
    */

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
    //fileAudio.appendChild( doc.createTextNode( m_audio_buffer.buffer().toBase64() ) );
    fileAudio.appendChild( doc.createTextNode( Riff( m_format ).rawData( m_audio_buffer ).toBase64() ) );
//    fileAudio.appendChild( doc.createTextNode("uuuu") );
    files.appendChild( fileAudio );

    QDomElement fileImage = doc.createElement("File");
    fileImage.setAttribute("Name", "image.jpg");
    fileImage.appendChild( doc.createTextNode( buf.buffer().toBase64() ) );
//    fileImage.appendChild( doc.createTextNode("jjjj") );
    files.appendChild( fileImage );

    element.appendChild( files );

    root.appendChild( element );

//    QDomElement img = doc.createElement("image");
//    root.appendChild( img );
//    //img.setAttribute("data", "der image");

//    QDomText t = doc.createTextNode( buf.buffer().toBase64() );
//    img.appendChild( t );


//    QDomElement aud = doc.createElement("audio");
//    root.appendChild( aud );

//    QDomText a = doc.createTextNode( m_audio_buffer.buffer().toBase64() );
//    aud.appendChild( a );

    QDomNode pi = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    doc.insertBefore( pi, doc.firstChild() );

    qDebug() << doc.toString();

    // отправить

    QUrl url( server );
    url.setPort(20001);
//    QUrl url( "http://192.168.0.160/quanon.php");
    QNetworkRequest request( url );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QVariant("application/xml") );

    emit sendBegin();

    m_reply = m_nam->post( request, doc.toByteArray() );

    connect( m_reply, SIGNAL( error( QNetworkReply::NetworkError ) ),
             SLOT( replyError( QNetworkReply::NetworkError ) ) );
    connect( m_reply, SIGNAL( sslErrors( QList< QSslError> ) ),
             SLOT( replySslErrors( QList< QSslError > ) ) );
    connect( m_reply, SIGNAL( finished() ), SLOT( replyFinished() ) );
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
    qDebug() << "replyFinished";
    m_reply->deleteLater();
    emit sendEnd();
}
