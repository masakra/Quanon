#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QAudioFormat>
#include <QObject>
#include <QBuffer>
#include <QImage>
#include <QNetworkReply>

class QAudioInput;
class QNetworkAccessManager;

class Processor : public QObject
{
    Q_OBJECT

    private:
        QImage m_image;

        QAudioInput * m_audio_in;

        QAudioFormat m_format;

        QBuffer m_audio_buffer;

        void audioCapture();

        QNetworkAccessManager * m_nam;

        QNetworkReply * m_reply;

        QString m_server;

        int m_rowid;

    private Q_SLOTS:
        void replyError( QNetworkReply::NetworkError error );
        void replySslErrors( QList< QSslError > errorList );
        void replyFinished();

    public:
        explicit Processor(QObject *parent = 0);
        Q_INVOKABLE void save( const QString & author );

        /*! \fn  void send( const QString & server )
         *
         * \brief Sends one latest message from SQLite3 database
         */
        Q_INVOKABLE void send( const QString & server );

    public Q_SLOTS:
        void processImage( const QString & path );
        void stopAudioRecord();

    Q_SIGNALS:
        void sendBegin();
        void sendEnd();
        void message( const QString & text ) const;
};

#endif // PROCESSOR_H
