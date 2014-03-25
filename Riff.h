
#ifndef RIFF_H
#define RIFF_H

#include <QtGlobal>


#define RIFF_SIZE 44

class QAudioFormat;
class QBuffer;

class Riff
{
	private:
		char chunkId[ 4 ];			// 1
		quint32 chunkSize;			// 2
		char format[ 4 ];			// 3
		char subchunk1Id[ 4 ];		// 4
		quint32 subchunk1Size;		// 5
		quint16 audioFormat;		// 6
		quint16 numChannels;		// 7
		quint32 sampleRate;			// 8
		quint32 byteRate;			// 9
		quint16 blockAlign;			// 10
		quint16 bitsPerSample;		// 11
		char subchunk2Id[ 4 ];		// 12
		quint32 subchunk2Size;		// 13


		void setData( const QBuffer & b );

	public:
		Riff();

		void setAudioFormat( const QAudioFormat & f );


		QByteArray rawData( const QBuffer & b );
};

#endif

