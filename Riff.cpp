
#include "Riff.h"

#include <QAudioFormat>
#include <QBuffer>

Riff::Riff()
	: subchunk1Size( 16 ),		// 5
	audioFormat( 1 )			// 6	1 == PCM
{
	// 1
	chunkId[ 0 ] = 'R';
	chunkId[ 1 ] = 'I';
	chunkId[ 2 ] = 'F';
	chunkId[ 3 ] = 'F';
	// 2
	// 3
	format[ 0 ] = 'W';
	format[ 1 ] = 'A';
	format[ 2 ] = 'V';
	format[ 3 ] = 'E';
	// 4
	subchunk1Id[ 0 ] = 'f';
	subchunk1Id[ 1 ] = 'm';
	subchunk1Id[ 2 ] = 't';
	subchunk1Id[ 3 ] = ' ';
	// 7
	// 8
	// 9
	// 10
	// 11
	// 12
	subchunk2Id[ 0 ] = 'd';
	subchunk2Id[ 1 ] = 'a';
	subchunk2Id[ 2 ] = 't';
	subchunk2Id[ 3 ] = 'a';
	// 13

}

void
Riff::setAudioFormat( const QAudioFormat & f )
{
	// 7
	numChannels = f.channelCount();
	// 8
	sampleRate = f.sampleRate();
	// 9
	byteRate = f.sampleRate() * f.channelCount() * f.sampleSize() / 8;
	// 10
	blockAlign = f.channelCount() * f.sampleSize() / 8;
	// 11
	bitsPerSample = f.sampleSize();
}

void
Riff::setData( const QBuffer & b )
{
	// 2
	chunkSize = b.size() + RIFF_SIZE - 8;
	// 13
	subchunk2Size = b.size();
}

QByteArray
Riff::rawData( const QBuffer & b )
{
	if ( b.data().isEmpty() )
		return QByteArray();

	setData( b );

	QByteArray ba( reinterpret_cast< const char * >( this ), RIFF_SIZE );

	ba.append( b.buffer() );

	return ba;
}


