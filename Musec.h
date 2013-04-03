#ifndef Musec_h
#define Musec_h

#include "ui_musec.h"

class QMediaPlayer;
class QTimer;

class Musec : public QWidget, private Ui::MusecMainWindow {
    Q_OBJECT

public:
    Musec(QWidget* parent = 0);

private:
    QMediaPlayer* fPlayer;
    QTimer* fTimer;
    QStringList fSongs;
    QString fDir;
    qint64 fStartTime;
    void loadSong(QString filename);
    void playSong();

private slots:
    void timeout();
    void on_btnPlay_clicked();
    void on_btnNext_clicked();
    void on_btnBrowse_clicked();
};
#endif
