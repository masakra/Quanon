#include "processor.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickImageProvider>

#include <QAudioRecorder>
//#include <QAudioInput>
//#include <QBuffer>

#include <QDebug>

Processor::Processor(QObject *parent) :
    QObject(parent)
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
    QImage image = imageProvider->requestImage( imageId, &imageSize, imageSize );

    if ( ! image.isNull() )
        qDebug() << image.size();
    else
        qDebug() << "imageIsNull";

    audioCapture();
}

void
Processor::audioCapture()
{
//    QByteArray output_bytes;
//    QAudioInput * audio_in;
//    QAudioFormat format;
//    QBuffer output_buffer;

    QAudioRecorder * recorder = new QAudioRecorder;

    QAudioEncoderSettings settings;

    settings.setCodec("audio/amr");
    settings.setQuality( QMultimedia::HighQuality );

    recorder->setEncodingSettings( settings );

    recorder->setOutputLocation( QUrl::fromLocalFile("~/test.amr") );

}
