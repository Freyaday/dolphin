// Copyright 2017 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#pragma once

#include <QDialog>
#include <QString>
#include <memory>

#include "InputCommon/ControllerInterface/Device.h"

namespace ControllerEmu
{
class EmulatedController;
}

class InputConfig;
class QComboBox;
class QDialogButtonBox;
class QEvent;
class QHBoxLayout;
class QGroupBox;
class QVBoxLayout;
class QPushButton;
class QTabWidget;
class QWidget;

class MappingWindow final : public QDialog
{
  Q_OBJECT
public:
  enum class Type
  {
    // GameCube
    MAPPING_GC_BONGOS,
    MAPPING_GC_DANCEMAT,
    MAPPING_GC_KEYBOARD,
    MAPPING_GCPAD,
    MAPPING_GCPAD_WIIU,
    MAPPING_GC_STEERINGWHEEL,
    // Wii
    MAPPING_WIIMOTE_EMU,
    MAPPING_WIIMOTE_HYBRID
  };

  explicit MappingWindow(QWidget* parent, int port_num);
  void ChangeMappingType(Type type);

  int GetPort() const;
  const ciface::Core::DeviceQualifier& GetDeviceQualifier() const;
  std::shared_ptr<ciface::Core::Device> GetDevice() const;

  void SetBlockInputs(const bool block);
signals:
  void Update();
  void ClearFields();

private:
  void CreateDevicesLayout();
  void CreateProfilesLayout();
  void CreateResetLayout();
  void CreateMainLayout();
  void ConnectWidgets();
  void LoadSettings();

  void SetLayoutComplex(bool is_complex);
  void AddWidget(const QString& name, QWidget* widget);
  void ClearWidgets();

  void RefreshDevices();

  void OnDeleteProfilePressed();
  void OnLoadProfilePressed();
  void OnSaveProfilePressed();
  void OnDefaultFieldsPressed();
  void OnProfileChanged(int index);
  void OnDeviceChanged(int index);

  bool event(QEvent* event) override;

  ControllerEmu::EmulatedController* m_controller = nullptr;

  // Main
  QVBoxLayout* m_main_layout;
  QHBoxLayout* m_config_layout;
  QDialogButtonBox* m_button_box;

  // Devices
  QGroupBox* m_devices_box;
  QHBoxLayout* m_devices_layout;
  QComboBox* m_devices_combo;
  QPushButton* m_devices_refresh;

  // Profiles
  QGroupBox* m_profiles_box;
  QVBoxLayout* m_profiles_layout;
  QComboBox* m_profiles_combo;
  QPushButton* m_profiles_load;
  QPushButton* m_profiles_save;
  QPushButton* m_profiles_delete;

  // Reset
  QGroupBox* m_reset_box;
  QVBoxLayout* m_reset_layout;
  QPushButton* m_reset_default;
  QPushButton* m_reset_clear;

  QTabWidget* m_tab_widget;

  Type m_mapping_type;
  const int m_port;
  bool m_is_complex;
  bool m_block = false;
  InputConfig* m_config;
  ciface::Core::DeviceQualifier m_devq;
};
