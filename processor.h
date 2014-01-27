#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QObject>
#include <QBuffer>
#include <QImage>

class QAudioInput;
class QNetworkAccessManager;

class Processor : public QObject
{
    Q_OBJECT
    private:
        QImage m_image;

        QAudioInput * m_audio_in;

        QBuffer m_audio_buffer;

        void audioCapture();

        QNetworkAccessManager * nam;

    public:
        explicit Processor(QObject *parent = 0);

    public Q_SLOTS:
        void processImage( const QString & path );
        void stopAudioRecord();
        void send( const QString & author );
};

#endif // PROCESSOR_H
