#include "AudioPlayer.h"
#include <QUrl>
#include <QDebug>

AudioPlayer* AudioPlayer::s_instance = nullptr;

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , m_player(new QMediaPlayer(this))
    , m_audioOutput(new QAudioOutput(this))
{
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(80);

    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this, [](QMediaPlayer::PlaybackState state) {
                if (state == QMediaPlayer::StoppedState) {
                    qDebug() << "Audio playback stopped";
                }
            });

    qDebug() << "AudioPlayer initialized";
}

AudioPlayer::~AudioPlayer()
{
    if (m_player) {
        m_player->stop();
    }
    qDebug() << "AudioPlayer destroyed";
}

AudioPlayer* AudioPlayer::instance()
{
    if (!s_instance) {
        s_instance = new AudioPlayer();
    }
    return s_instance;
}

void AudioPlayer::destroy()
{
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

void AudioPlayer::play(const QString &filePath)
{
    if (!filePath.isEmpty() &&
        (filePath.endsWith(".wav") || filePath.endsWith(".mp3"))) {
        // Stop any currently playing audio
        m_player->stop();

        // Set the new source and play
        m_player->setSource(QUrl(filePath));
        m_player->play();
    } else {
        qWarning() << "Sound file not found:" << filePath;
    }
}

void AudioPlayer::setVolume(int volume)
{
    if (m_audioOutput) {
        m_audioOutput->setVolume(qBound(0, volume, 100));
    }
}

void AudioPlayer::stop()
{
    m_player->stop();
}
