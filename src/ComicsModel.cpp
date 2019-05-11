/**
 * Copyright (c) 2015 Damien Tardy-Panis
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

#include "ComicsModel.h"

#include <QDebug>
#include <QDate>
#include <QColor>

#include "Comic.h"
#include "ComicFactory.h"
#include "ComicDatabaseResource.h"
#include "Settings.h"

ComicsModel::ComicsModel(QObject *parent) :
    QAbstractListModel(parent),
    m_list(QList<Comic*>()),
    m_automaticFetchTimer(NULL)
{
    dbResource = ComicDatabaseResource::instance();
    factory    = ComicFactory::instance();
    settings   = Settings::instance();

    connect(this, &ComicsModel::modelReset, this, &ComicsModel::emitCountsChanged);
    connect(this, &ComicsModel::rowsInserted, this, &ComicsModel::emitCountsChanged);
    connect(this, &ComicsModel::rowsRemoved, this, &ComicsModel::emitCountsChanged);
}

ComicsModel::~ComicsModel()
{
    delete m_automaticFetchTimer;
    clear();
}

void ComicsModel::clear()
{
    beginResetModel();
    qDeleteAll(m_list);
    m_list.clear();
    endResetModel();
}

QHash<int, QByteArray> ComicsModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[IdRole] = "id";
    roleNames[RandomRole] = "random";
    roleNames[NameRole] = "name";
    roleNames[SortNameRole] = "sortName";
    roleNames[ColorRole] = "idColor";
    roleNames[AuthorsRole] = "authors";
    roleNames[HomepageRole] = "homepage";
    roleNames[LanguageRole] = "language";
    roleNames[CoverRole] = "coverPath";
    roleNames[ExampleRole] = "examplePath";
    roleNames[ImageRole] = "image";
    roleNames[ImageUrlRole] = "imageUrl";
    roleNames[FavoriteRole] = "favorite";
    roleNames[NewStripRole] = "newStrip";
    roleNames[ErrorRole] = "error";
    roleNames[AnimatedRole] = "imageAnimated";
    roleNames[FetchingRole] = "fetching";
    roleNames[FetchingProgressRole] = "fetchingProgress";

    return roleNames;
}

int ComicsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant ComicsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_list.size())
        return QVariant();

    switch(role) {
    case IdRole:
        return m_list.at(index.row())->id();
    case RandomRole:
        return m_list.at(index.row())->random();
    case NameRole:
        return m_list.at(index.row())->name();
    case SortNameRole:
        return m_list.at(index.row())->sortName();
    case ColorRole:
        return m_list.at(index.row())->color();
    case AuthorsRole:
        return m_list.at(index.row())->authors();
    case HomepageRole:
        return m_list.at(index.row())->homepage();
    case LanguageRole:
        return m_list.at(index.row())->language();
    case CoverRole:
        return m_list.at(index.row())->coverPath();
    case ExampleRole:
        return m_list.at(index.row())->examplePath();
    case ImageRole:
        return m_list.at(index.row())->stripImagePath();
    case ImageUrlRole:
        return m_list.at(index.row())->stripImageUrl();
    case FavoriteRole:
        return m_list.at(index.row())->favorite();
    case NewStripRole:
        return m_list.at(index.row())->newStrip();
    case ErrorRole:
        return m_list.at(index.row())->error();
    case AnimatedRole:
        return m_list.at(index.row())->animated();
    case FetchingRole:
        return m_list.at(index.row())->fetching();
    case FetchingProgressRole:
        return m_list.at(index.row())->fetchingProgress();
    }

    return QVariant();
}

void ComicsModel::loadAll()
{
    clear();

    QStringList comicsList = idLoadList();

    Comic *comic;

    beginInsertRows(QModelIndex(), 0, comicsList.count() - 1);

    for (int i = 0; i < comicsList.size(); ++i) {
        comic = factory->create(comicsList.at(i), this);

        if (comic == NULL)
            return;

        comic->load();

        connect(comic, &Comic::favoriteChanged, this, &ComicsModel::emitFavoriteChanged);
        connect(comic, &Comic::newStripChanged, this, &ComicsModel::emitNewStripChanged);
        connect(comic, &Comic::errorChanged, this, &ComicsModel::emitErrorChanged);
        connect(comic, &Comic::fetchingChanged, this, &ComicsModel::emitFetchingChanged);
        connect(comic, &Comic::fetchingProgressChanged, this, &ComicsModel::emitFetchingProgressChanged);

        m_list.append(comic);
    }

    endInsertRows();

    // hack to fix the initialization of the cover...
    emit countChanged();
}

void ComicsModel::emitCountsChanged()
{
    emit countChanged();
    emit favoritesCountChanged();
    emit newCountChanged();
    emit fetchedCountChanged();
    emit errorCountChanged();
}

QStringList ComicsModel::idLoadList()
{
    return factory->fullList();
}

bool ComicsModel::emitDataChanged(Comic *comic, ComicsModel::Roles role)
{
    for(int row = 0; row < m_list.size(); ++row) {
        if (m_list.at(row) == comic) {
            QVector<int> roleVector;
            roleVector << role;
            emit dataChanged(index(row), index(row), roleVector);
            return true;
        }
    }

    return false;
}

void ComicsModel::emitFavoriteChanged(Comic *comic)
{
    bool result = emitDataChanged(comic, FavoriteRole);
    if (result) emit favoritesCountChanged();
}

void ComicsModel::emitNewStripChanged(Comic *comic)
{
    bool result = emitDataChanged(comic, NewStripRole);
    if (result) emit newCountChanged();
}

void ComicsModel::emitErrorChanged(Comic *comic)
{
    bool result = emitDataChanged(comic, ErrorRole);
    if (result) emit errorCountChanged();
}

void ComicsModel::emitFetchingChanged(Comic *comic)
{
    bool result = emitDataChanged(comic, FetchingRole);
    if (result) emit fetchedCountChanged();
}

void ComicsModel::emitFetchingProgressChanged(Comic *comic)
{
    emitDataChanged(comic, FetchingProgressRole);
}

void ComicsModel::fetchAll()
{
    for(int row = 0; row < m_list.size(); ++row) {
        m_list.at(row)->fetchStrip();
    }
}

void ComicsModel::setFavorite(int row, bool favorite)
{
    m_list.at(row)->setFavorite(favorite);
}

void ComicsModel::read(int row)
{
    m_list.at(row)->read();
}

void ComicsModel::favoriteAll(bool favorite)
{
    for(int row = 0; row < m_list.size(); ++row) {
        setFavorite(row, favorite);
    }
}

void ComicsModel::unfavoriteAll()
{
    favoriteAll(false);
}

void ComicsModel::saveAll()
{
    QStringList favoriteIds;

    for(int row = 0; row < m_list.size(); ++row) {
        if (m_list.at(row)->favorite())
            favoriteIds.append(m_list.at(row)->id());
    }

    dbResource->saveFavorites(favoriteIds);
}

Comic *ComicsModel::comicAt(int row)
{
    if (row < 0 || row >= m_list.size())
        return NULL;

    return m_list.at(row);
}

QStringList ComicsModel::errorComicNames()
{
    QStringList(errorComicNames);

    for(int row = 0; row < m_list.size(); ++row) {
      if (m_list.at(row)->error())
          errorComicNames << m_list.at(row)->name();
    }

    return errorComicNames;
}

void ComicsModel::startAutomaticFetch(int interval)
{
    m_automaticFetchTimer = new QTimer(this);
    m_automaticFetchTimer->start(interval);
    connect(m_automaticFetchTimer, &QTimer::timeout, this, &ComicsModel::fetchAll);
}

int ComicsModel::count() const
{
    return rowCount();
}

int ComicsModel::roleCount(ComicsModel::Roles role, QVariant value) const
{
    int count = 0;

    for (int row = 0; row < m_list.size(); ++row) {
      if (data(index(row), role) == value)
          count++;
    }

    return count;
}

int ComicsModel::favoritesCount() const
{
    return roleCount(FavoriteRole, true);
}

int ComicsModel::newCount() const
{
    return roleCount(NewStripRole, true);
}

int ComicsModel::fetchedCount() const
{
    return roleCount(FetchingRole, false);
}

int ComicsModel::errorCount() const
{
    return roleCount(ErrorRole, true);
}
