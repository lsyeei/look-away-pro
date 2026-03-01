#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QString>

class AudioPlayer : public QObject
{
    Q_OBJECT

public:
    static AudioPlayer* instance();
    static void destroy();

    void play(const QString &filePath);
    void setVolume(int volume);
    void stop();

private:
    explicit AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();

    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    static AudioPlayer *s_instance;
};

#endif // AUDIOPLAYER_H
