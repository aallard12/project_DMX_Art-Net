/****************************************************************************
** Meta object code from reading C++ file 'interfacepcclient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../Documents/InterfacePcClient7/interfacepcclient.h"
#include <QtNetwork/QSslError>
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'interfacepcclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN17InterfacePcClientE_t {};
} // unnamed namespace

template <> constexpr inline auto InterfacePcClient::qt_create_metaobjectdata<qt_meta_tag_ZN17InterfacePcClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "InterfacePcClient",
        "on_btnGoToList_clicked",
        "",
        "on_btnAddMain_clicked",
        "on_btnGoToScenes_clicked",
        "on_btnGoToLive_clicked",
        "on_filterUniversCombo_currentIndexChanged",
        "index",
        "on_btnAddUnivers_clicked",
        "refreshUniversList",
        "on_btnEditUnivers_clicked",
        "on_btnDeleteUnivers_clicked",
        "on_btnGoToConnect_clicked",
        "on_uiUniversList_currentRowChanged",
        "currentRow",
        "clearForm",
        "on_pushButtonCouleur_clicked",
        "on_btnAddChannelGlobal_clicked",
        "on_btnCancel_clicked",
        "on_btnSave_clicked",
        "on_pushButtonImport_clicked",
        "on_scenesUniversCombo_currentIndexChanged",
        "on_scenesCombo_currentIndexChanged",
        "on_btnResetSliders_clicked",
        "on_btnRenameScene_clicked",
        "on_btnDeleteScene_clicked",
        "on_btnSaveScene_clicked",
        "refreshScenesList",
        "on_btnConnectTCP_clicked",
        "onQTcpSocket_connected",
        "onQTcpSocket_disconnected",
        "on_liveScenesList_itemSelectionChanged",
        "on_btnLaunchLiveScene_clicked",
        "on_liveUniversCombo_currentIndexChanged",
        "refreshLiveScenesList",
        "idUniversFiltre",
        "on_pushButtonSaveUser_clicked",
        "refreshUserList",
        "on_listWidgetUser_currentRowChanged",
        "on_pushButtonEditUser_clicked",
        "on_pushButtonDeleteUser_clicked",
        "onAnalyseClaudeTerminee",
        "equipementJson",
        "onErreurClaude",
        "message"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'on_btnGoToList_clicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnAddMain_clicked'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGoToScenes_clicked'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGoToLive_clicked'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_filterUniversCombo_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Slot 'on_btnAddUnivers_clicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshUniversList'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnEditUnivers_clicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnDeleteUnivers_clicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGoToConnect_clicked'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_uiUniversList_currentRowChanged'
        QtMocHelpers::SlotData<void(int)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 },
        }}),
        // Slot 'clearForm'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_pushButtonCouleur_clicked'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnAddChannelGlobal_clicked'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnCancel_clicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnSave_clicked'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_pushButtonImport_clicked'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_scenesUniversCombo_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Slot 'on_scenesCombo_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Slot 'on_btnResetSliders_clicked'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnRenameScene_clicked'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnDeleteScene_clicked'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnSaveScene_clicked'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshScenesList'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnConnectTCP_clicked'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onQTcpSocket_connected'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onQTcpSocket_disconnected'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_liveScenesList_itemSelectionChanged'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnLaunchLiveScene_clicked'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_liveUniversCombo_currentIndexChanged'
        QtMocHelpers::SlotData<void(int)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Slot 'refreshLiveScenesList'
        QtMocHelpers::SlotData<void(int)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 35 },
        }}),
        // Slot 'on_pushButtonSaveUser_clicked'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'refreshUserList'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_listWidgetUser_currentRowChanged'
        QtMocHelpers::SlotData<void(int)>(38, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 },
        }}),
        // Slot 'on_pushButtonEditUser_clicked'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_pushButtonDeleteUser_clicked'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAnalyseClaudeTerminee'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(41, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QJsonObject, 42 },
        }}),
        // Slot 'onErreurClaude'
        QtMocHelpers::SlotData<void(const QString &)>(43, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 44 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<InterfacePcClient, qt_meta_tag_ZN17InterfacePcClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject InterfacePcClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17InterfacePcClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17InterfacePcClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17InterfacePcClientE_t>.metaTypes,
    nullptr
} };

void InterfacePcClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<InterfacePcClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->on_btnGoToList_clicked(); break;
        case 1: _t->on_btnAddMain_clicked(); break;
        case 2: _t->on_btnGoToScenes_clicked(); break;
        case 3: _t->on_btnGoToLive_clicked(); break;
        case 4: _t->on_filterUniversCombo_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->on_btnAddUnivers_clicked(); break;
        case 6: _t->refreshUniversList(); break;
        case 7: _t->on_btnEditUnivers_clicked(); break;
        case 8: _t->on_btnDeleteUnivers_clicked(); break;
        case 9: _t->on_btnGoToConnect_clicked(); break;
        case 10: _t->on_uiUniversList_currentRowChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->clearForm(); break;
        case 12: _t->on_pushButtonCouleur_clicked(); break;
        case 13: _t->on_btnAddChannelGlobal_clicked(); break;
        case 14: _t->on_btnCancel_clicked(); break;
        case 15: _t->on_btnSave_clicked(); break;
        case 16: _t->on_pushButtonImport_clicked(); break;
        case 17: _t->on_scenesUniversCombo_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 18: _t->on_scenesCombo_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 19: _t->on_btnResetSliders_clicked(); break;
        case 20: _t->on_btnRenameScene_clicked(); break;
        case 21: _t->on_btnDeleteScene_clicked(); break;
        case 22: _t->on_btnSaveScene_clicked(); break;
        case 23: _t->refreshScenesList(); break;
        case 24: _t->on_btnConnectTCP_clicked(); break;
        case 25: _t->onQTcpSocket_connected(); break;
        case 26: _t->onQTcpSocket_disconnected(); break;
        case 27: _t->on_liveScenesList_itemSelectionChanged(); break;
        case 28: _t->on_btnLaunchLiveScene_clicked(); break;
        case 29: _t->on_liveUniversCombo_currentIndexChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 30: _t->refreshLiveScenesList((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 31: _t->on_pushButtonSaveUser_clicked(); break;
        case 32: _t->refreshUserList(); break;
        case 33: _t->on_listWidgetUser_currentRowChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 34: _t->on_pushButtonEditUser_clicked(); break;
        case 35: _t->on_pushButtonDeleteUser_clicked(); break;
        case 36: _t->onAnalyseClaudeTerminee((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 37: _t->onErreurClaude((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *InterfacePcClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *InterfacePcClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17InterfacePcClientE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int InterfacePcClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 38)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 38;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 38)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 38;
    }
    return _id;
}
QT_WARNING_POP
