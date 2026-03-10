#include "AudioPlayer.h"
#include <QUrl>
#include <QDebug>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QProcess>
#include <QRegularExpression>

#ifdef Q_OS_WIN
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#endif

AudioPlayer* AudioPlayer::s_instance = nullptr;

static float getSystemVolume()
{
#ifdef Q_OS_WIN
    HRESULT hr;
    IMMDeviceEnumerator *pEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    IAudioEndpointVolume *pEndpointVolume = nullptr;
    float volume = 1.0f;

    CoInitialize(nullptr);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                          __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) {
        qWarning() << "Failed to create device enumerator";
        goto cleanup;
    }

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hr)) {
        qWarning() << "Failed to get default audio endpoint";
        goto cleanup;
    }

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                           (void**)&pEndpointVolume);
    if (FAILED(hr)) {
        qWarning() << "Failed to activate audio endpoint volume";
        goto cleanup;
    }

    hr = pEndpointVolume->GetMasterVolumeLevelScalar(&volume);
    if (FAILED(hr)) {
        qWarning() << "Failed to get system volume";
        volume = 1.0f;
    } else {
        qDebug() << "System volume:" << volume;
    }

cleanup:
    if (pEndpointVolume) pEndpointVolume->Release();
    if (pDevice) pDevice->Release();
    if (pEnumerator) pEnumerator->Release();
    CoUninitialize();

    return volume;
#elif defined(Q_OS_MACOS)
    // macOS 使用 AppleScript 获取系统音量
    QProcess process;
    process.start("osascript", QStringList() << "-e" << "output volume of (get volume settings)");
    process.waitForFinished();
    QString output = process.readAllStandardOutput().trimmed();
    float volume = output.toFloat() / 100.0f;
    qDebug() << "macOS system volume:" << volume;
    return qBound(0.0f, volume, 1.0f);
#elif defined(Q_OS_LINUX)
    // Linux 尝试使用 pulseaudio 或 alsa 获取系统音量
    // 这里使用 pactl 作为示例（需要安装 pulseaudio-utils）
    QProcess process;
    process.start("pactl", QStringList() << "get-sink-volume" << "@DEFAULT_SINK@");
    if (process.waitForFinished()) {
        QString output = process.readAllStandardOutput();
        QRegularExpression re(R"(\/\s*(\d+)%\s*\/)");
        QRegularExpressionMatch match = re.match(output);
        if (match.hasMatch()) {
            float volume = match.captured(1).toFloat() / 100.0f;
            qDebug() << "Linux system volume:" << volume;
            return qBound(0.0f, volume, 1.0f);
        }
    }
    qWarning() << "Failed to get Linux system volume, using default";
    return 1.0f;
#else
    // 其他平台使用默认音量
    qWarning() << "System volume detection not supported on this platform, using default";
    return 1.0f;
#endif
}

AudioPlayer::AudioPlayer(QObject *parent)
    : QObject(parent)
    , m_player(new QMediaPlayer(this))
    , m_audioOutput(new QAudioOutput(this))
{
    m_player->setAudioOutput(m_audioOutput);
    float systemVolume = getSystemVolume();
    m_audioOutput->setVolume(systemVolume);

    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this, [](QMediaPlayer::PlaybackState state) {
                if (state == QMediaPlayer::StoppedState) {
                    qDebug() << "Audio playback stopped";
                }
            });

    qDebug() << "AudioPlayer initialized with system volume:" << systemVolume;
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
        m_audioOutput->setVolume(qBound(0.0, volume / 100.0, 1.0));
    }
}

void AudioPlayer::stop()
{
    m_player->stop();
}
