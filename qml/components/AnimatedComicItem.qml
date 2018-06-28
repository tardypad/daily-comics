/**
 * Copyright (c) 2018 Oleg Linkin
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 **/

import QtQuick 2.0
import Sailfish.Silica 1.0

import harbour.dailycomics.Comics 1.0

import "../utils"
import "../components"

import "../scripts/ExternalLinks.js" as ExternalLinks

Item {
    anchors.fill: parent

    SilicaFlickable {
        id: comicView

        anchors.fill: parent

        contentWidth: width
        contentHeight: Math.max(comicImage.height + 2 * Theme.paddingLarge, parent.height)

        AnimatedImage {
            id: comicImage
            source: !comic.error && !indicator.busy ? comic.stripImagePath : ''

            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            smooth: true
            clip: true
            asynchronous: true

            width: parent.width
            height: (sourceSize.height / sourceSize.width) * width

            Rectangle {
                anchors.fill: parent
                color: "white"
                visible: status == Image.Ready
                z: -10
            }

            onStatusChanged: {
                if (status === Image.Ready)
                    comic.read()
                else if (status === Image.Error) {
                    indicator.displayError(qsTr("Image error"), qsTr("Can't display strip"))
                    comic.setError()
                }
            }
        }

        LoadingIndicator {
            id: indicator
            model: comic
            flickable: comicView
            loadingText: qsTr("Loading comic")
            defaultErrorText: qsTr("Can't display comic")
            networkErrorText: qsTr("Can't download comic")
            parsingErrorText: qsTr("Can't extract comic")
            savingErrorText: qsTr("Can't save comic")
        }

        ErrorContactDevRectangle {
            comicName: comic.name
            flickable: comicView
            active: indicator.error
            z: 10
        }

        BusyIndicator {
            anchors.centerIn: parent
            running: comicImage.status === Image.Loading
            size: BusyIndicatorSize.Large
        }

        PullDownMenu {
            MenuItem {
                visible: !indicator.busy
                text: qsTr("Report a problem with the comic")
                onClicked: ExternalLinks.mail(constants.devMail, constants.mailErrorSubjectHeader,
                                              constants.mailBodyHeader + "There is a problem with comic \"" + encodeURIComponent(comic.name) + "\"")
            }
            MenuItem {
                text: qsTr("Copy url to clipboard")
                onClicked: Clipboard.text = comic.stripImageUrl
            }
            MenuItem {
                text: qsTr("Show comic info")
                onClicked: comicView._showComicInfo()
            }
            MenuItem {
                text: qsTr("Go to homepage")
                onClicked: ExternalLinks.browse(comic.homepage)
            }
        }

        function _showComicInfo() {
            if (infoPanelLoader.status === Loader.Null) {
                infoPanelLoader.source = Qt.resolvedUrl("../components/ComicInfoPanel.qml")
                infoPanelLoader.item.parent = comicPage
                infoPanelLoader.item.index = index
                infoPanelLoader.item.comicsModel = comicsModel
                infoPanelLoader.item.homepageMenu = false
            }
            infoPanelLoader.item.showComicInfo()
        }
    }

    Loader {
        id: infoPanelLoader
    }
}