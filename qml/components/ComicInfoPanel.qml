/**
 * Copyright (c) 2015 Damien Tardy-Panis
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

import QtQuick 2.0
import Sailfish.Silica 1.0

import harbour.dailycomics.Comics 1.0

import "../scripts/ExternalLinks.js" as ExternalLinks

DockedPanel {
    id: comicInfoPanel

    property int index
    property ComicsModel comicsModel
    property bool isPortrait: (parent.isPortrait !== undefined) ? parent.isPortrait : true
    property alias homepageMenu: homepagePushUpMenu.visible

    width: isPortrait ? parent.width : parent.height
    height: isPortrait
            ? infoColumn.height + 2*Theme.paddingMedium
            : parent.height
    contentHeight: height
    dock: isPortrait ? Dock.Bottom : Dock.Right

    function showComicInfo() {
        comic.setComic(comicsModel.comicAt(index))
        show()
    }

    ComicProxy {
        id: comic
    }

    MouseArea {
        anchors.fill: parent
        onClicked: hide()
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.rgba(Theme.highlightDimmerColor, 0.9)

        Column {
            id: infoColumn
            spacing: Theme.paddingMedium
            anchors.centerIn: parent
            width: parent.width - 2 * Theme.paddingMedium

            Label {
                id: nameLabel
                text: comic.name
                width: parent.width
                font.pixelSize: Theme.fontSizeMedium
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Grid {
                columns: 2
                spacing: Theme.paddingMedium

                Label {
                    id: authorsHeaderLabel
                    text: comic.authors.length > 1 ? qsTr("Authors") : qsTr("Author")
                    font {
                        italic: true
                        pixelSize: Theme.fontSizeExtraSmall
                    }
                    color: Theme.secondaryColor
                }

                Label {
                    id: authorsLabel
                    text: comic.authors.join("\n")
                    font.pixelSize: Theme.fontSizeSmall
                    truncationMode: TruncationMode.Fade
                }

                Label {
                    id: languageHeaderLabel
                    text: qsTr("Language")
                    font {
                        italic: true
                        pixelSize: Theme.fontSizeExtraSmall
                    }
                    color: Theme.secondaryColor
                }

                Label {
                    id: languageLabel
                    text: comic.language
                    font.pixelSize: Theme.fontSizeSmall
                    truncationMode: TruncationMode.Fade
                }
            }

            Image {
                id: exampleImage
                source: comic.id ? comic.examplePath : ''
                fillMode: Image.PreserveAspectFit
                smooth: true
                asynchronous: true

                width: parent.width
                height: isPortrait
                        ? Math.min(window.height / 4, implicitHeight)
                        : comicInfoPanel.height - (nameLabel.height + 7 * Theme.paddingMedium
                          + authorsLabel.height + languageLabel.height)

                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

    }

    PushUpMenu {
        id: homepagePushUpMenu

        MenuItem {
            text: qsTr("Go to homepage")
            onClicked: ExternalLinks.browse(comic.homepage)
        }
    }
}
