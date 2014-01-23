#include "processor.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickImageProvider>

#include <QAudioFormat>
#include <QAudioRecorder>
#include <QAudioInput>
#include <QBuffer>
#include <QJsonObject>

#include <QDebug>

Processor::Processor(QObject *parent) :
    QObject(parent), m_audio_in( 0 )
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
Processor::send()
{
    stopAudioRecord();

    QJsonObject json;

//    QByteArray img;
    QBuffer buf;//( &img );
    buf.open( QIODevice::WriteOnly );

    if ( ! m_image.save( &buf, "PNG" ) ) {
        qWarning() << "Error image to buffer writing";
        return;
    }

    json.insert("img", QJsonValue::fromVariant( buf.buffer() ) );

    json.insert("audio", QJsonValue::fromVariant( m_audio_buffer.buffer() ) );

    // JSON готов к отправке
}
