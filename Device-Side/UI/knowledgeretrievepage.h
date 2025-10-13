#ifndef KNOWLEDGERETRIEVEPAGE_H
#define KNOWLEDGERETRIEVEPAGE_H

#include <QWidget>
#include "softkeyboard.h"

namespace Ui {
class KnowledgeRetrievePage;
}

enum VoiceStatus {
    RAW = 0,
    TAKING
};

enum ResourceType {
    TEXT = 0,
    IMAGE,
    MARKDOWN
};

class DocumentWidget: public QWidget
{
    Q_OBJECT
public:
    explicit DocumentWidget(QWidget *parent = nullptr);
    ~DocumentWidget();
    void setContent(const QString &msg);
    void setImage(const QString &url);
    void setMarkdown(const QString &text);
    void setPrompt(const QString &prompt);

    ResourceType type() const { return type_; }

    QString getContent()const { return content_; };
    QString getMarkdown() const { return content_; }
    QPixmap getImage() const { return pix_; }

protected:
    void paintEvent(QPaintEvent *ev) override;

private:
    QLabel *label_;
    ResourceType type_{ResourceType::TEXT};
    QString content_;
    QPixmap pix_;
    /* TODO: other content(image、text ...) */

};

class KnowledgeRetrievePage : public QWidget
{
    Q_OBJECT

public:
    explicit KnowledgeRetrievePage(QWidget *parent = nullptr);
    ~KnowledgeRetrievePage();

    void clearText();

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    bool eventFilter(QObject *watched, QEvent *ev) override;

private:
    Ui::KnowledgeRetrievePage *ui;
    VoiceStatus curstatus_{VoiceStatus::RAW};
    QString voiceButtonStyles[2];
    SoftKeyboard *keyboard_;
    std::vector<DocumentWidget*> documents_;
    QLabel *showLabel_;
    QPixmap initPixmap_;

    void renderDocument(const DocumentWidget *widget);
    void zoom(bool type);

private slots:
    void voiceRecord();
    void sendAndRetrieve();
};

#endif // KNOWLEDGERETRIEVEPAGE_H
