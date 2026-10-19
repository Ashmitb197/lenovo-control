import QtQuick
import QtQuick.Layouts
import LenovoControl.Backend 1.0
import org.kde.plasma.plasmoid
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents

PlasmoidItem {
    id: root

    readonly property var supportedProfiles:
        hardware.capabilities.performanceProfiles || []

    readonly property bool supportsConservation:
        hardware.capabilities.conservationInterface === true

    preferredRepresentation: compactRepresentation

    HardwareBridge {
        id: hardware
    }

    compactRepresentation: MouseArea {
        implicitWidth: 28
        implicitHeight: 28

        onClicked: root.expanded = !root.expanded

        Kirigami.Icon {
            anchors.centerIn: parent
            width: 22
            height: 22
            source: "computer-laptop"
        }
    }

    fullRepresentation: ColumnLayout {
        Layout.minimumWidth: 340
        Layout.preferredWidth: 380
        Layout.minimumHeight: 300

        spacing: 16

        PlasmaComponents.Label {
            text: "Lenovo Control"
            font.bold: true
            font.pixelSize: 18
        }

        RowLayout {
            Layout.fillWidth: true

            ColumnLayout {
                PlasmaComponents.Label {
                    text: "Battery"
                    opacity: 0.7
                }

                PlasmaComponents.Label {
                    text: hardware.battery
                    font.pixelSize: 24
                    font.bold: true
                }
            }

            Item {
                Layout.fillWidth: true
            }

            ColumnLayout {
                PlasmaComponents.Label {
                    text: "CPU Temperature"
                    opacity: 0.7
                }

                PlasmaComponents.Label {
                    text: hardware.temperature
                    font.pixelSize: 24
                    font.bold: true
                }
            }
        }

        PlasmaComponents.Label {
            text: "Performance Profile"
            font.bold: true
            visible: root.supportedProfiles.length > 0
        }

        RowLayout {
            Layout.fillWidth: true
            visible: root.supportedProfiles.length > 0

            PlasmaComponents.Button {
                text: "Quiet"
                visible: root.supportedProfiles.indexOf("low-power") !== -1
                Layout.fillWidth: true
                highlighted: hardware.profile === "low-power"
                onClicked: hardware.setProfile("low-power")
            }

            PlasmaComponents.Button {
                text: "Balanced"
                visible: root.supportedProfiles.indexOf("balanced") !== -1
                Layout.fillWidth: true
                onClicked: hardware.setProfile("balanced")
                highlighted: hardware.profile.toLowerCase() === "balanced"
            }

            PlasmaComponents.Button {
                text: "Performance"
                visible: root.supportedProfiles.indexOf("performance") !== -1
                Layout.fillWidth: true
                highlighted: hardware.profile === "performance"
                onClicked: hardware.setProfile("performance")
            }
        }

        RowLayout {
            Layout.fillWidth: true
            visible: root.supportsConservation

            PlasmaComponents.Label {
                text: "Battery Conservation"
            }

            Item {
                Layout.fillWidth: true
            }

            PlasmaComponents.Label {
                text: hardware.conservation
            }
        }

        Item {
            Layout.fillHeight: true
        }

        PlasmaComponents.Button {
            text: "Open Full Application"
            Layout.fillWidth: true
            onClicked: hardware.openApplication()
        }
    }
}
