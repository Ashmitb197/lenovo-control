import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window

    width: 1060
    height: 690
    minimumWidth: 850
    minimumHeight: 560
    visible: true

    title: "Lenovo Control"
    color: "#0d1016"

    property color surface: "#171d28"
    property color accent: "#6b9cff"
    property color muted: "#9ca9bd"
    property string notification: "System ready"

    component StatCard: Rectangle {
        property string heading: ""
        property string reading: ""
        property string description: ""

        color: window.surface
        radius: 16
        border.color: "#293244"
        border.width: 1

        implicitHeight: 155

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 12

            Label {
                text: heading
                color: window.muted
                font.pixelSize: 13
            }

            Label {
                text: reading
                color: "#f1f5ff"
                font.pixelSize: 25
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            Label {
                text: description
                color: window.muted
                font.pixelSize: 12
            }

            Item { Layout.fillHeight: true }
        }
    }

    component ActionButton: Button {
        id: control

        property bool emphasized: false

        implicitHeight: 44
        implicitWidth: 130

        contentItem: Text {
            text: control.text
            color: control.enabled ? "#f3f6ff" : "#697488"
            font.pixelSize: 13
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            radius: 10

            color: !control.enabled ? "#252a34"
                 : control.down ? "#365c9a"
                 : control.hovered ? "#34496a"
                 : control.emphasized ? "#315caa"
                 : "#273142"

            border.color: control.emphasized ? "#5989e0" : "#364358"
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 215
            Layout.fillHeight: true

            color: "#141a24"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 18

                Label {
                    text: "LENOVO\nCONTROL"
                    color: "#f2f5fc"
                    font.pixelSize: 21
                    font.bold: true
                }

                Label {
                    text: "SYSTEM UTILITY"
                    color: window.muted
                    font.pixelSize: 10
                    font.letterSpacing: 2
                }

                Item { Layout.preferredHeight: 25 }

                ActionButton {
                    text: "Dashboard"
                    emphasized: true
                    Layout.fillWidth: true
                }

                Item { Layout.fillHeight: true }

                Label {
                    text: "v0.4.0 · Native C++"
                    color: window.muted
                    font.pixelSize: 11
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: Math.max(0, window.width - 215 - 60)
                x: 30
                spacing: 22

                Item { Layout.preferredHeight: 14 }

                Label {
                    text: "System Overview"
                    color: "#f3f6ff"
                    font.pixelSize: 29
                    font.bold: true
                }

                Label {
                    text: "Your hardware, performance and battery at a glance."
                    color: window.muted
                    font.pixelSize: 13
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 14

                    StatCard {
                        Layout.fillWidth: true
                        heading: "CPU TEMPERATURE"
                        reading: hardware.temperature
                        description: "Live thermal reading"
                    }

                    StatCard {
                        Layout.fillWidth: true
                        heading: "MEMORY"
                        reading: hardware.memory
                        description: "Current RAM usage"
                    }

                    StatCard {
                        Layout.fillWidth: true
                        heading: "BATTERY"
                        reading: hardware.battery
                        description: "Current battery status"
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: profileLayout.implicitHeight + 44
                    color: window.surface
                    radius: 16
                    border.color: "#293244"

                    ColumnLayout {
                        id: profileLayout
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: 22
                        spacing: 17

                        Label {
                            text: "Performance Profile"
                            color: "#f2f5fc"
                            font.pixelSize: 19
                            font.bold: true
                        }

                        Label {
                            text: "Current: " + hardware.profile
                            color: window.muted
                        }

                        RowLayout {
                            spacing: 12

                            Repeater {
                                model: [
                                    { label: "Quiet", mode: "low-power" },
                                    { label: "Balanced", mode: "balanced" },
                                    { label: "Performance", mode: "performance" }
                                ]

                                ActionButton {
                                    required property var modelData

                                    text: modelData.label
                                    enabled: !hardware.busy
                                    emphasized: hardware.profile === modelData.mode

                                    onClicked: hardware.setProfile(modelData.mode)
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: batteryLayout.implicitHeight + 44
                    color: window.surface
                    radius: 16
                    border.color: "#293244"

                    ColumnLayout {
                        id: batteryLayout
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: 22
                        spacing: 17

                        Label {
                            text: "Battery Conservation"
                            color: "#f2f5fc"
                            font.pixelSize: 19
                            font.bold: true
                        }

                        Label {
                            text: "Current: " + hardware.conservation
                            color: window.muted
                        }

                        RowLayout {
                            spacing: 12

                            ActionButton {
                                text: "Enable"
                                enabled: !hardware.busy
                                emphasized: hardware.conservation === "Enabled"
                                onClicked: hardware.setConservation(true)
                            }

                            ActionButton {
                                text: "Disable"
                                enabled: !hardware.busy
                                emphasized: hardware.conservation === "Disabled"
                                onClicked: hardware.setConservation(false)
                            }
                        }
                    }
                }

                Label {
                    text: window.notification
                    color: window.muted
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }

                Item { Layout.preferredHeight: 20 }
            }
        }
    }

    Connections {
        target: hardware

        function onOperationFinished(success, message) {
            window.notification = message
        }
    }
}
