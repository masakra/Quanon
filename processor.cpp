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

#include <QDebug>

Processor::Processor(QObject *parent) :
    QObject(parent), m_audio_in( 0 ), nam( new QNetworkAccessManager( this ) )
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
//    QByteArray output_bytes;
//    QAudioInput * audio_in;
    QAudioFormat format;
//    QBuffer output_buffer;

//    format.setFrequency( 8000 );
//    format.setChannels( 1 );
    format.setSampleSize( 16 );
    format.setCodec("audio/pcm");
    format.setByteOrder( QAudioFormat::LittleEndian );
    format.setSampleType( QAudioFormat::UnSignedInt );

    m_audio_buffer.open( QIODevice::WriteOnly | QIODevice::Truncate );

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();

    if ( ! info.isFormatSupported( format ) ) {
        qWarning() << "default format do not supported, use nearest";
        format = info.nearestFormat( format );
    }

    m_audio_in = new QAudioInput( format, this );
    m_audio_in->start( &m_audio_buffer );

    /*
    QAudioRecorder * recorder = new QAudioRecorder;

    QAudioEncoderSettings settings;

    settings.setCodec("audio/amr");
    settings.setQuality( QMultimedia::HighQuality );

    recorder->setEncodingSettings( settings );

    recorder->setOutputLocation( QUrl::fromLocalFile("~/test.amr") );
    */
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
Processor::send( const QString & author )
{
    stopAudioRecord();

    QJsonObject json;

//    QByteArray img;
    QBuffer buf;//( &img );
    buf.open( QIODevice::WriteOnly );

    if ( ! m_image.save( &buf, "PNG" ) ) {
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

    QDomElement root = doc.createElement("quanon");
    doc.appendChild( root );

    QDomElement aut = doc.createElement("author");
    root.appendChild( aut );

    QDomText m = doc.createTextNode( author );
    aut.appendChild( m );

    QDomElement img = doc.createElement("image");
    root.appendChild( img );
    //img.setAttribute("data", "der image");

    QDomText t = doc.createTextNode( buf.buffer().toBase64() );
    img.appendChild( t );


    QDomElement aud = doc.createElement("audio");
    root.appendChild( aud );

    QDomText a = doc.createTextNode( m_audio_buffer.buffer().toBase64() );
    aud.appendChild( a );

    doc.createProcessingInstruction("xml", "version\"1.0\" encoding=\"utf-8\"");

    qDebug() << doc.toString();

    // отправить

    QNetworkRequest request( QUrl("http://192.168.0.160/quanon.php") );
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/xml" );

    nam->post( request, doc.toByteArray() );


}
