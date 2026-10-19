import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    id: popup

    width: 340
    height: 365
    visible: false
    title: "Lenovo Control"
    color: "#151922"

    flags: Qt.Tool | Qt.FramelessWindowHint


    function showPopup() {
        popup.show()
        popup.raise()
        popup.requestActivate()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 14

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: "LENOVO CONTROL"
                color: "#ffffff"
                font.bold: true
                font.pixelSize: 15
            }

            Item { Layout.fillWidth: true }

            Label {
                text: hardware.profile
                color: "#a5b4fc"
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            radius: 12
            color: "#222938"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 14

                ColumnLayout {
                    Label {
                        text: "Battery"
                        color: "#9ca3af"
                    }

                    Label {
                        text: hardware.battery
                        color: "white"
                        font.pixelSize: 19
                        font.bold: true
                    }
                }

                Item { Layout.fillWidth: true }

                ColumnLayout {
                    Label {
                        text: "CPU Temperature"
                        color: "#9ca3af"
                    }

                    Label {
                        text: {
                            const value = hardware.temperature
                            if (!hardware.useFahrenheit)
                                return value

                            const c = parseFloat(value)
                            return Number.isFinite(c)
                                ? (c * 9 / 5 + 32).toFixed(1) + " °F"
                                : value
                        }
                        color: "white"
                        font.pixelSize: 19
                        font.bold: true
                    }
                }
            }
        }

        Label {
            text: "PERFORMANCE MODE"
            color: "#9ca3af"
            font.pixelSize: 11
            font.bold: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Repeater {
                model: [
                    { label: "Quiet", mode: "low-power" },
                    { label: "Balanced", mode: "balanced" },
                    { label: "Performance", mode: "performance" }
                ]

                delegate: Button {
                    required property var modelData

                    Layout.fillWidth: true

                    text: modelData.label
                    highlighted: hardware.profile === modelData.mode

                    onClicked: hardware.setProfile(modelData.mode)
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 55
            radius: 12
            color: "#222938"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12

                Label {
                    text: "Battery Conservation"
                    color: "white"
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: hardware.conservation
                    color: "#a5b4fc"
                }
            }
        }

        Item { Layout.fillHeight: true }
    }
}
