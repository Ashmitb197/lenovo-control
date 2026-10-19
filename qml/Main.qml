import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    visible: true
    width: 1120
    height: 740
    minimumWidth: 820
    minimumHeight: 560
    title: "Lenovo Control"
    color: "#0c1017"

    readonly property color panel: "#171e29"
    readonly property color borderColor: "#2b3545"
    readonly property color accent: "#83aaff"
    readonly property color foreground: "#f1f4fa"
    readonly property color muted: "#94a2b7"

    property int currentPage: 0
    property string notification: "System ready"

    component VectorIcon: Item {
        id: icon
        property string kind: "chip"
        property color stroke: window.accent
        implicitWidth: 22
        implicitHeight: 22

        Canvas {
            anchors.fill: parent
            antialiasing: true

            onPaint: {
                const ctx = getContext("2d")
                ctx.reset()
                ctx.strokeStyle = icon.stroke.toString()
                ctx.fillStyle = icon.stroke.toString()
                ctx.lineWidth = 1.7
                ctx.lineCap = "round"
                ctx.lineJoin = "round"

                const sx = width / 24
                const sy = height / 24
                ctx.scale(sx, sy)

                function line(x1,y1,x2,y2) {
                    ctx.beginPath()
                    ctx.moveTo(x1,y1)
                    ctx.lineTo(x2,y2)
                    ctx.stroke()
                }

                function rect(x,y,w,h) {
                    ctx.strokeRect(x,y,w,h)
                }

                if (icon.kind === "chip") {
                    rect(6,6,12,12)
                    rect(9,9,6,6)
                    for (let n = 8; n <= 16; n += 4) {
                        line(n,3,n,6)
                        line(n,18,n,21)
                        line(3,n,6,n)
                        line(18,n,21,n)
                    }
                } else if (icon.kind === "memory") {
                    rect(3,6,18,12)
                    for (let n = 6; n <= 18; n += 4) {
                        line(n,3,n,6)
                        line(n,18,n,21)
                    }
                    rect(7,9,10,6)
                } else if (icon.kind === "battery") {
                    rect(3,6,17,12)
                    ctx.fillRect(21,10,2,4)
                    rect(6,9,10,6)
                } else if (icon.kind === "dashboard") {
                    rect(3,3,8,8)
                    rect(13,3,8,8)
                    rect(3,13,8,8)
                    rect(13,13,8,8)
                } else if (icon.kind === "performance") {
                    ctx.beginPath()
                    ctx.moveTo(13,2)
                    ctx.lineTo(5,13)
                    ctx.lineTo(11,13)
                    ctx.lineTo(10,22)
                    ctx.lineTo(19,10)
                    ctx.lineTo(13,10)
                    ctx.closePath()
                    ctx.stroke()
                } else if (icon.kind === "check") {
                    line(4,12,10,18)
                    line(10,18,20,6)
                }
            }

            Component.onCompleted: requestPaint()
        }

        onKindChanged: {
            for (let i = 0; i < children.length; ++i)
                if (children[i].requestPaint)
                    children[i].requestPaint()
        }
    }

    component NavItem: Rectangle {
        id: nav
        property string label: ""
        property string iconKind: "dashboard"
        property bool selected: false
        signal activated()

        Layout.fillWidth: true
        implicitHeight: 48
        radius: 11
        color: selected ? "#253957"
                        : area.containsMouse ? "#1b2737" : "transparent"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 12
            spacing: 13

            VectorIcon {
                kind: nav.iconKind
                stroke: nav.selected ? window.accent : window.muted
                Layout.preferredWidth: 19
                Layout.preferredHeight: 19
            }

            Text {
                text: nav.label
                color: nav.selected ? window.foreground : window.muted
                font.pixelSize: 13
                font.weight: nav.selected ? Font.DemiBold : Font.Normal
                Layout.fillWidth: true
            }
        }

        MouseArea {
            id: area
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: nav.activated()
        }

        Behavior on color { ColorAnimation { duration: 140 } }
    }


    component MetricCard: Rectangle {
        id: metric

        property string heading: ""
        property string reading: ""
        property string detail: ""
        property string iconKind: "chip"
        property real percentage: -1
        property color progressColor: window.accent

        Layout.fillWidth: true
        Layout.minimumWidth: 0
        implicitHeight: 166
        radius: 15
        color: window.panel
        border.color: window.borderColor

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 18
            spacing: 9

            RowLayout {
                Layout.fillWidth: true

                Rectangle {
                    width: 36
                    height: 36
                    radius: 10
                    color: "#263854"

                    VectorIcon {
                        anchors.centerIn: parent
                        kind: metric.iconKind
                    }
                }

                Item { Layout.fillWidth: true }

                Rectangle {
                    width: 6
                    height: 6
                    radius: 3
                    color: metric.progressColor
                }
            }

            Text {
                text: metric.heading
                color: window.muted
                font.pixelSize: 11
                font.weight: Font.DemiBold
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Text {
                text: metric.reading
                color: window.foreground
                font.pixelSize: metric.reading.length > 15 ? 17 : 23
                font.weight: Font.DemiBold
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 5
                radius: 3
                color: "#303b4c"
                visible: metric.percentage >= 0

                Rectangle {
                    width: parent.width *
                        Math.max(0, Math.min(100, metric.percentage)) / 100
                    height: parent.height
                    radius: parent.radius
                    color: metric.progressColor

                    Behavior on width {
                        NumberAnimation {
                            duration: 350
                            easing.type: Easing.OutCubic
                        }
                    }
                }
            }

            Text {
                text: metric.detail
                color: window.muted
                font.pixelSize: 11
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Item { Layout.fillHeight: true }
        }
    }


    component ModeButton: Rectangle {
        id: mode

        property string label: ""
        property string detail: ""
        property string iconKind: "performance"
        property bool selected: false
        property bool available: true

        signal activated()

        Layout.fillWidth: true
        Layout.minimumWidth: 0
        implicitHeight: 86

        radius: 12
        color: selected ? "#294267"
             : mouse.containsMouse && available ? "#283649"
             : "#202a38"

        border.width: 1
        border.color: selected ? window.accent : "#364457"
        opacity: available ? 1 : 0.45

        RowLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 10

            Rectangle {
                Layout.preferredWidth: 34
                Layout.preferredHeight: 34
                radius: 9
                color: selected ? "#36598a" : "#2b394b"

                VectorIcon {
                    anchors.centerIn: parent
                    width: 19
                    height: 19
                    kind: mode.iconKind
                    stroke: selected ? "#a8c6ff" : window.muted
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5

                Text {
                    text: mode.label
                    color: window.foreground
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }

                Text {
                    text: mode.detail
                    color: window.muted
                    font.pixelSize: 10
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                }
            }

            VectorIcon {
                visible: mode.selected
                kind: "check"
                stroke: window.accent
                Layout.preferredWidth: 17
                Layout.preferredHeight: 17
            }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            enabled: mode.available
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: mode.activated()
        }

        Behavior on color {
            ColorAnimation { duration: 160 }
        }

        Behavior on border.color {
            ColorAnimation { duration: 160 }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            color: "#121923"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 12

                RowLayout {
                    spacing: 12

                    Rectangle {
                        width: 40
                        height: 40
                        radius: 11
                        color: "#29466e"

                        Text {
                            anchors.centerIn: parent
                            text: "L"
                            color: "white"
                            font.pixelSize: 21
                            font.bold: true
                        }
                    }

                    Column {
                        spacing: 3

                        Text {
                            text: "LENOVO"
                            color: window.foreground
                            font.pixelSize: 15
                            font.bold: true
                        }

                        Text {
                            text: "CONTROL"
                            color: window.muted
                            font.pixelSize: 10
                            font.letterSpacing: 2
                        }
                    }
                }

                Item { Layout.preferredHeight: 35 }

                Text {
                    text: "WORKSPACE"
                    color: window.muted
                    font.pixelSize: 10
                    font.letterSpacing: 1.5
                }

                NavItem {
                    label: "Overview"
                    iconKind: "dashboard"
                    selected: window.currentPage === 0
                    onActivated: window.currentPage = 0
                }

                NavItem {
                    label: "Performance"
                                iconKind: "performance"
                    selected: window.currentPage === 1
                    onActivated: window.currentPage = 1
                }

                NavItem {
                    label: "Battery"
                    iconKind: "battery"
                    selected: window.currentPage === 2
                    onActivated: window.currentPage = 2
                }

                NavItem {
                    label: "Hardware"
                    iconKind: "chip"
                    selected: window.currentPage === 3
                    onActivated: window.currentPage = 3
                }

                Item { Layout.fillHeight: true }

                Text {
                    text: "NATIVE EDITION"
                    color: window.accent
                    font.pixelSize: 11
                    font.bold: true
                }

                Text {
                    text: "C++20  ·  Qt 6  ·  v0.4.0"
                    color: window.muted
                    font.pixelSize: 11
                }
            }
        }

        ScrollView {
            id: scroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            ColumnLayout {
                // Critical fix: size against ScrollView's actual viewport,
                // not the implicit width of the scrollable content.
                width: scroll.availableWidth
                spacing: 20

                Item { Layout.preferredHeight: 12 }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 28
                    Layout.rightMargin: 28
                    spacing: 12

                    ColumnLayout {
                        spacing: 6

                        Text {
                            text: window.currentPage === 0 ? "System Overview"
                                : window.currentPage === 1 ? "Performance Center"
                                : window.currentPage === 2 ? "Battery Management"
                                : "Hardware Monitoring"
                            color: window.foreground
                            font.pixelSize: 27
                            font.weight: Font.DemiBold
                        }

                        Text {
                            text: "Hardware control and system monitoring"
                            color: window.muted
                            font.pixelSize: 12
                        }
                    }

                    Item { Layout.fillWidth: true }

                    RowLayout {
                        spacing: 8

                        Rectangle {
                            width: 7
                            height: 7
                            radius: 4
                            color: "#61d5a3"
                        }

                        Text {
                            text: "LIVE"
                            color: "#83ddb7"
                            font.pixelSize: 11
                            font.bold: true
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 28
                    Layout.rightMargin: 28
                    visible: window.currentPage !== 3
                    spacing: 14

                    MetricCard {
                        heading: "CPU TEMPERATURE"
                        reading: hardware.temperature
                        detail: "Processor sensor"
                        iconKind: "chip"
                    }

                    MetricCard {
                        heading: "SYSTEM MEMORY"
                        percentage: {
                            const parts = hardware.memory.split(" / ")
                            if (parts.length !== 2)
                                return -1
                            const used = parseFloat(parts[0])
                            const total = parseFloat(parts[1])
                            return total > 0 ? 100 * used / total : -1
                        }
                        reading: hardware.memory
                        detail: "RAM usage"
                        iconKind: "memory"
                    }

                    MetricCard {
                        heading: "BATTERY STATUS"
                        progressColor: "#65cda6"
                        percentage: {
                            const value = parseFloat(hardware.battery)
                            return isNaN(value) ? -1 : value
                        }
                        reading: hardware.battery
                        detail: "Current charge"
                        iconKind: "battery"
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.leftMargin: 28
                    Layout.rightMargin: 28
                    implicitHeight: performanceLayout.implicitHeight + 44
                    visible: window.currentPage !== 2 && window.currentPage !== 3
                    radius: 15
                    color: window.panel
                    border.color: window.borderColor

                    ColumnLayout {
                        id: performanceLayout
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: 22
                        spacing: 16

                        Text {
                            text: "Performance Profiles"
                            color: window.foreground
                            font.pixelSize: 18
                            font.bold: true
                        }

                        Text {
                            text: "Current profile: " + hardware.profile
                            color: window.muted
                            font.pixelSize: 12
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            ModeButton {
                                label: "Quiet"
                                iconKind: "battery"
                                detail: "Lower power"
                                selected: hardware.profile === "low-power"
                                available: !hardware.busy
                                onActivated: hardware.setProfile("low-power")
                            }

                            ModeButton {
                                label: "Balanced"
                                iconKind: "dashboard"
                                detail: "Everyday use"
                                selected: hardware.profile === "balanced"
                                available: !hardware.busy
                                onActivated: hardware.setProfile("balanced")
                            }

                            ModeButton {
                                label: "Performance"
                                iconKind: "performance"
                                detail: "Higher power"
                                selected: hardware.profile === "performance"
                                available: !hardware.busy
                                onActivated: hardware.setProfile("performance")
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.leftMargin: 28
                    Layout.rightMargin: 28
                    implicitHeight: batteryLayout.implicitHeight + 44
                    visible: window.currentPage !== 1 && window.currentPage !== 3
                    radius: 15
                    color: window.panel
                    border.color: window.borderColor

                    ColumnLayout {
                        id: batteryLayout
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: 22
                        spacing: 16

                        Text {
                            text: "Battery Conservation"
                            color: window.foreground
                            font.pixelSize: 18
                            font.bold: true
                        }

                        Text {
                            text: "Limit charging to help preserve battery health."
                            color: window.muted
                            font.pixelSize: 12
                        }

                        Text {
                            text: "Current state: " + hardware.conservation
                            color: window.accent
                            font.pixelSize: 12
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            ModeButton {
                                label: "Enabled"
                                iconKind: "battery"
                                detail: "Conservation on"
                                selected: hardware.conservation === "Enabled"
                                available: !hardware.busy
                                onActivated: hardware.setConservation(true)
                            }

                            ModeButton {
                                label: "Disabled"
                                iconKind: "battery"
                                detail: "Normal charging"
                                selected: hardware.conservation === "Disabled"
                                available: !hardware.busy
                                onActivated: hardware.setConservation(false)
                            }
                        }
                    }
                }


                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 28
                    Layout.rightMargin: 28
                    spacing: 18
                    visible: window.currentPage === 3

                    Text {
                        text: "PROCESSOR"
                        color: window.muted
                        font.pixelSize: 11
                        font.bold: true
                        font.letterSpacing: 1.5
                    }

                    Text {
                        text: hardware.cpuModel
                        color: window.foreground
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 14

                        MetricCard {
                            heading: "CPU UTILIZATION"
                            reading: hardware.cpuUsage
                            detail: "Processor load"
                            iconKind: "chip"
                            percentage: parseFloat(hardware.cpuUsage)
                        }

                        MetricCard {
                            heading: "CPU TEMPERATURE"
                            reading: hardware.temperature
                            detail: "Processor sensor"
                            iconKind: "chip"
                        }
                    }

                    Text {
                        text: "INTEL INTEGRATED GRAPHICS"
                        color: window.muted
                        font.pixelSize: 11
                        font.bold: true
                        font.letterSpacing: 1.5
                    }

                    Text {
                        text: hardware.intelGpuModel
                        color: window.foreground
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 14

                        MetricCard {
                            heading: "CURRENT GPU CLOCK"
                            reading: hardware.intelGpuFrequency
                            detail: "Live Intel GPU frequency"
                            iconKind: "chip"
                        }

                        MetricCard {
                            heading: "MAXIMUM GPU CLOCK"
                            reading: hardware.intelGpuMaxFrequency
                            detail: "Configured maximum frequency"
                            iconKind: "performance"
                        }
                    }

                    Text {
                        text: "Intel GPU utilization is not available through the detected sysfs interfaces."
                        color: window.muted
                        font.pixelSize: 12
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    Text {
                        text: "NVIDIA GRAPHICS"
                        color: window.muted
                        font.pixelSize: 11
                        font.bold: true
                        font.letterSpacing: 1.5
                    }

                    Text {
                        text: hardware.gpuModel
                        color: window.foreground
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 14

                        MetricCard {
                            heading: "GPU UTILIZATION"
                            reading: hardware.gpuUsage
                            detail: "NVIDIA GPU load"
                            iconKind: "performance"
                            percentage: parseFloat(hardware.gpuUsage)
                        }

                        MetricCard {
                            heading: "GPU TEMPERATURE"
                            reading: hardware.gpuTemperature
                            detail: "NVIDIA temperature"
                            iconKind: "chip"
                        }
                    }

                    MetricCard {
                        heading: "VIDEO MEMORY"
                        reading: hardware.gpuMemory
                        detail: "Dedicated GPU memory"
                        iconKind: "memory"
                    }

                    Text {
                        text: "BATTERY HEALTH"
                        color: window.muted
                        font.pixelSize: 11
                        font.bold: true
                        font.letterSpacing: 1.5
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 14

                        MetricCard {
                            heading: "BATTERY HEALTH"
                            reading: hardware.batteryHealth
                            detail: "Capacity compared with original"
                            iconKind: "battery"
                            progressColor: "#65cda6"
                            percentage: parseFloat(hardware.batteryHealth)
                        }

                        MetricCard {
                            heading: "CHARGE CYCLES"
                            reading: hardware.batteryCycles
                            detail: "Battery cycle count"
                            iconKind: "battery"
                        }
                    }
                }

                Text {
                    text: window.notification
                    color: window.muted
                    font.pixelSize: 12
                    Layout.fillWidth: true
                    Layout.leftMargin: 28
                    Layout.rightMargin: 28
                    wrapMode: Text.WordWrap
                }

                Item { Layout.preferredHeight: 16 }
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
