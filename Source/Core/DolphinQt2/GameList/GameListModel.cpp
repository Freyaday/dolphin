// Copyright 2015 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include "DolphinQt2/GameList/GameListModel.h"
#include "DolphinQt2/Resources.h"
#include "DolphinQt2/Settings.h"

const QSize GAMECUBE_BANNER_SIZE(96, 32);

GameListModel::GameListModel(QObject* parent) : QAbstractTableModel(parent)
{
  connect(&m_tracker, &GameTracker::GameLoaded, this, &GameListModel::UpdateGame);
  connect(&m_tracker, &GameTracker::GameRemoved, this, &GameListModel::RemoveGame);
  connect(this, &GameListModel::DirectoryAdded, &m_tracker, &GameTracker::AddDirectory);
  connect(this, &GameListModel::DirectoryRemoved, &m_tracker, &GameTracker::RemoveDirectory);

  connect(&Settings::Instance(), &Settings::ThemeChanged, [this] {
    // Tell the view to repaint. The signal 'dataChanged' also seems like it would work here, but
    // unfortunately it won't cause a repaint until the view is focused.
    emit layoutAboutToBeChanged();
    emit layoutChanged();
  });
}

QVariant GameListModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  QSharedPointer<GameFile> game = m_games[index.row()];

  switch (index.column())
  {
  case COL_PLATFORM:
    if (role == Qt::DecorationRole)
      return Resources::GetPlatform(static_cast<int>(game->GetPlatformID()));
    if (role == Qt::InitialSortOrderRole)
      return static_cast<int>(game->GetPlatformID());
    break;
  case COL_COUNTRY:
    if (role == Qt::DecorationRole)
      return Resources::GetCountry(static_cast<int>(game->GetCountryID()));
    if (role == Qt::InitialSortOrderRole)
      return static_cast<int>(game->GetCountryID());
    break;
  case COL_RATING:
    if (role == Qt::DecorationRole)
      return Resources::GetRating(game->GetRating());
    if (role == Qt::InitialSortOrderRole)
      return game->GetRating();
    break;
  case COL_BANNER:
    if (role == Qt::DecorationRole)
    {
      // GameCube banners are 96x32, but Wii banners are 192x64.
      // TODO: use custom banners from rom directory like DolphinWX?
      QPixmap banner = game->GetBanner();
      banner.setDevicePixelRatio(std::max(banner.width() / GAMECUBE_BANNER_SIZE.width(),
                                          banner.height() / GAMECUBE_BANNER_SIZE.height()));
      return banner;
    }
    break;
  case COL_TITLE:
    if (role == Qt::DisplayRole || role == Qt::InitialSortOrderRole)
      return game->GetLongName();
    break;
  case COL_ID:
    if (role == Qt::DisplayRole || role == Qt::InitialSortOrderRole)
      return game->GetGameID();
    break;
  case COL_DESCRIPTION:
    if (role == Qt::DisplayRole || role == Qt::InitialSortOrderRole)
      return game->GetDescription();
    break;
  case COL_MAKER:
    if (role == Qt::DisplayRole || role == Qt::InitialSortOrderRole)
      return game->GetMaker();
    break;
  case COL_SIZE:
    if (role == Qt::DisplayRole)
      return FormatSize(game->GetFileSize());
    if (role == Qt::InitialSortOrderRole)
      return game->GetFileSize();
    break;
  }

  return QVariant();
}

QVariant GameListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical || role != Qt::DisplayRole)
    return QVariant();

  switch (section)
  {
  case COL_TITLE:
    return tr("Title");
  case COL_ID:
    return tr("ID");
  case COL_BANNER:
    return tr("Banner");
  case COL_DESCRIPTION:
    return tr("Description");
  case COL_MAKER:
    return tr("Maker");
  case COL_SIZE:
    return tr("Size");
  case COL_RATING:
    return tr("Quality");
  }
  return QVariant();
}

int GameListModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return m_games.size();
}

int GameListModel::columnCount(const QModelIndex& parent) const
{
  return NUM_COLS;
}

void GameListModel::UpdateGame(QSharedPointer<GameFile> game)
{
  QString path = game->GetFilePath();

  int entry = FindGame(path);
  if (entry < 0)
    entry = m_games.size();
  else
    return;

  beginInsertRows(QModelIndex(), entry, entry);
  m_games.insert(entry, game);
  endInsertRows();
}

void GameListModel::RemoveGame(const QString& path)
{
  int entry = FindGame(path);
  if (entry < 0)
    return;

  beginRemoveRows(QModelIndex(), entry, entry);
  m_games.removeAt(entry);
  endRemoveRows();
}

int GameListModel::FindGame(const QString& path) const
{
  for (int i = 0; i < m_games.size(); i++)
  {
    if (m_games[i]->GetFilePath() == path)
      return i;
  }
  return -1;
}
