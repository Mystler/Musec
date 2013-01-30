#ifndef Musec_h
#define Musec_h

#include "ui_musec.h"

class QMediaPlayer;

class Musec : public QWidget, private Ui::MusecMainWindow {
    Q_OBJECT

public:
    Musec(QWidget* parent = 0);

private:
    QMediaPlayer* fPlayer;
    QStringList fSongs;
    void loadSong(QString filename);

public slots:
    

private slots:
    void on_btnPlay_clicked();
    void on_btnNext_clicked();
    void on_btnBrowse_clicked();
};
#endif
