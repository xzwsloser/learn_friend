#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include "knowledgeretrievepage.h"
#include "qtmaterialavatar.h"
#include "softkeyboard.h"

namespace Ui {
class chatPage;
}

enum ChatRole {
    HUMAN = 0,
    AI
};

enum ChatStatus {
    INIT = 0,
    CHATING
};


class ChatBubbleWidget: public QWidget {
    Q_OBJECT

public:
    explicit ChatBubbleWidget(ChatRole role,
                              QWidget *parent = nullptr);
    ~ChatBubbleWidget();

    void adjustBubbleSize();
    void setText(const QString &msg);
    void appendText(const QString &msg);
    void setMaxLineSize(int maxLineSize);

protected:
    void paintEvent(QPaintEvent *ev) override;

private:
    QtMaterialAvatar *avatar_;
    QLabel *content_;
    ChatRole role_{ChatRole::HUMAN};
    int maxLineSize_;

};

class chatPage : public QWidget
{
    Q_OBJECT

public:
    explicit chatPage(QWidget *parent = nullptr);
    ~chatPage();

protected:
    void paintEvent(QPaintEvent *ev) override;
    bool eventFilter(QObject *watched, QEvent *ev) override;

private:
    Ui::chatPage *ui;
    QString voiceButtonStyles[2];
    VoiceStatus voiceStatus_{VoiceStatus::RAW};
    SoftKeyboard *keyboard_;
    std::vector<ChatBubbleWidget*> msgRecords_;
    ChatStatus status_{ChatStatus::INIT};

private slots:
    void voiceRecord();
    void sendHumanMsg();
};

#endif // CHATPAGE_H
