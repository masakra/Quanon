#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QObject>

class Processor : public QObject
{
    Q_OBJECT
private:
    void audioCapture();

public:
    explicit Processor(QObject *parent = 0);

public Q_SLOTS:
    void processImage( const QString & path );


};

#endif // PROCESSOR_H
