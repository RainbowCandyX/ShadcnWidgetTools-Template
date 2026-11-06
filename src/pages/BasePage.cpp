#include "BasePage.h"

#include <QFont>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "AppI18n.h"
#include "ShadCard.h"
#include "ShadDef.h"
#include "ShadLabel.h"

BasePage::BasePage(AppI18n::TextFunction title, AppI18n::TextFunction description, QWidget* parent)
    : ShadScrollArea(parent)
{
    QWidget* contentWidget = new QWidget(this);
    _pageLayout = new QVBoxLayout(contentWidget);
    _pageLayout->setContentsMargins(32, 28, 32, 32);
    _pageLayout->setSpacing(16);

    ShadLabel* titleLabel = new ShadLabel(contentWidget);
    QFont titleFont = titleLabel->font();
    titleFont.setPixelSize(30);
    titleFont.setWeight(QFont::Bold);
    titleLabel->setFont(titleFont);

    ShadLabel* descriptionLabel = new ShadLabel(contentWidget);
    descriptionLabel->setColorRole(ShadThemeType::MutedForeground);
    QFont descriptionFont = descriptionLabel->font();
    descriptionFont.setPixelSize(16);
    descriptionFont.setWeight(QFont::Normal);
    descriptionLabel->setFont(descriptionFont);
    descriptionLabel->setWordWrap(true);

    AppI18n::bind(this, [this, titleLabel, descriptionLabel, title, description]()
    {
        const QString newTitle = title();
        titleLabel->setText(newTitle);
        descriptionLabel->setText(description());
        if (newTitle != _title)
        {
            _title = newTitle;
            Q_EMIT titleChanged(newTitle);
        }
    });

    _pageLayout->addWidget(titleLabel);
    _pageLayout->addWidget(descriptionLabel);
    _pageLayout->addSpacing(8);
    _pageLayout->addStretch(1);
    setWidget(contentWidget);
}

BasePage::~BasePage()
{
}

QString BasePage::getTitle() const
{
    return _title;
}

ShadCard* BasePage::addCard(AppI18n::TextFunction title, AppI18n::TextFunction description)
{
    ShadCard* card = new ShadCard(widget());
    AppI18n::bind(card, [card, title, description]()
    {
        card->setTitle(title());
        card->setDescription(description());
    });
    card->contentLayout()->setSpacing(12);
    // 末尾的 stretch 保持在最后，卡片从上往下排列
    _pageLayout->insertWidget(_pageLayout->count() - 1, card);
    return card;
}
