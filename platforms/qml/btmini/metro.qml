//import QtQuick 2.0
//import QtGraphicalEffects 1.0

//Rectangle {
//    width: 640
//    height: 480
//    color: "blue"

//    Item {
//        anchors.fill: parent

//        Rectangle {
//            id: rect
//            anchors.centerIn: parent
//            width: 100
//            height: 100
//            color: "red"
//        }
//    }

//    FastBlur {
//        anchors.fill: parent
//        cached: false
//        radius: 32.0
//        source: parent
//    }
//}


import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtGraphicalEffects 1.0

Rectangle {
    id:root
    width: 480
    height: 800

    property color colorBase:   '#ffffff'
    property color colorAccent: '#ff8800'
    property color colorText:   '#000000'
    property real speedMenu: 100
    property real tileWidth: 200


    Rectangle {
        width: root.width
        height: 100
        color: root.colorAccent
        id:panel
        state: 'default'

        states: [
            State {
                name: 'default'
            },
            State {
                name: 'down'
            },
            State {
                name: 'left'
            }
        ]

        transitions: [
            Transition {
                from: "default"
                to: "down"
                PropertyAnimation { target: panel; property: 'y'; to: 600; duration: 300; easing.type: Easing.OutCubic }
            },
            Transition {
                from: "down"
                to: "default"
                PropertyAnimation { target: panel; property: 'y'; to: 0; duration: 300; easing.type: Easing.OutCubic }
            },
            Transition {
                from: "default"
                to: "left"
                PropertyAnimation { target: panel; property: 'x'; to: -root.width; duration: 300; easing.type: Easing.OutCubic }
            },
            Transition {
                from: "left"
                to: "default"
                PropertyAnimation { target: panel; property: 'x'; to: 0; duration: 300; easing.type: Easing.OutCubic }
            }
        ]

        Rectangle {
            width: 130
            height: panel.height
            color: parent.color
            id: moduleRect

            Text {
                x: 16
                y: 44
                color: root.colorBase
                text: "KJV"
                font.pixelSize: 34
                font.family: "Tahoma"
            }

            MouseArea {
                anchors.fill: parent
                onPressed: PropertyAnimation { target: moduleRect; property: 'color'; to: root.colorText }
                onReleased: PropertyAnimation { target: moduleRect; property: 'color'; to: root.colorAccent }
                onClicked: {
                    panel.state = 'down'
                    fade.enabled = true
                }
            }
        }

        Rectangle {
            x: moduleRect.width
            width: 170
            height: panel.height
            color: parent.color
            id: placeRect

            Text {
                x: 16
                y: 44
                color: root.colorBase
                text: "Matt 5:1"
                font.pixelSize:34
                font.family: "Tahoma"
            }

            MouseArea {
                anchors.fill: parent
                onPressed: PropertyAnimation { target: placeRect; property: 'color'; to: root.colorText }
                onReleased: PropertyAnimation { target: placeRect; property: 'color'; to: root.colorAccent }
                onClicked: {
                    panel.state = 'down'
                    fade.enabled = true
                }
            }
        }

        Rectangle {
            width: panel.height
            x: panel.width - width
            height: panel.height
            color: parent.color
            id: controlRect

            Rectangle {
                id: controlPath
                width: parent.width * 0.8
                height: parent.height * 0.8
                anchors.centerIn: parent
                radius: width * 0.5
                border.color: root.colorBase
                border.width: 4
                color: "#00000000"
            }

            MouseArea {
                anchors.fill: parent
                onPressed: PropertyAnimation { target: controlRect; property: 'color'; to: root.colorText }
                onReleased: PropertyAnimation { target: controlRect; property: 'color'; to: root.colorAccent }
                onClicked: {
                    menuFindRect.state == 'on' ? menuFindRect.state = 'off' : menuFindRect.state = 'on';
                    fade.enabled = menuFindRect.state == 'on'
                }
            }
        }
    }

    Rectangle {
        width: root.width
        height: root.height - panel.height
        y: panel.y + panel.height
        x: panel.x
        id:text
        color: root.colorBase

        Flickable {
            anchors.fill: parent
            contentWidth: textItem.width; contentHeight: textItem.height
            clip: true

            Text {
                id: textItem
                width: root.width
                font.pixelSize:28
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                color: root.colorText
                text: '<h3>Introduction to the Sermon on the Mount</h3>
                       <p class="chapter-1"><sup class="versenum">5 </sup>Now when Jesus saw the crowds, he went up on a mountainside and sat down. His disciples came to him,
                       <sup class="versenum">2 </sup>and he began to teach them.</span></p><h3><span class="text Matt-5-2">The Beatitudes<sup class="crossreference" value="(<a href="#cen-NIV-23237A" title="See cross-reference A">A</a>)"></sup></span></h3><p class="first-line-none"><span class="text Matt-5-2">He said:</span></p> <div class="poetry top-05"><p class="line"><span id="en-NIV-23238" class="text Matt-5-3"><span class="woj"><sup class="versenum">3 </sup>“Blessed are the poor in spirit,</span></span><br /><span class="indent-1"><span class="indent-1-breaks">&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="text Matt-5-3"><span class="woj">for theirs is the kingdom of heaven.<sup class="crossreference" value="(<a href="#cen-NIV-23238B" title="See cross-reference B">B</a>)"></sup></span></span></span><br /><span id="en-NIV-23239" class="text Matt-5-4"><span class="woj"><sup class="versenum">4 </sup>Blessed are those who mourn,</span></span><br /><span class="indent-1"><span class="indent-1-breaks">&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="text Matt-5-4"><span class="woj">for they will be comforted.<sup class="crossreference" value="(<a href="#cen-NIV-23239C" title="See cross-reference C">C</a>)"></sup></span></span></span><br /><span id="en-NIV-23240" class="text Matt-5-5"><span class="woj"><sup class="versenum">5 </sup>Blessed are the meek,</span></span><br /><span class="indent-1"><span class="indent-1-breaks">&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="text Matt-5-5"><span class="woj">for they will inherit the earth.<sup class="crossreference" value="(<a href="#cen-NIV-23240D" title="See cross-reference D">D</a>)"></sup></span>
                       </span></span><br /><span id="en-NIV-23241" class="text Matt-5-6"><span class="woj"><sup class="versenum">6 </sup>Blessed are those who hunger and thirst for righteousness,</span></span><br /><span class="indent-1"><span class="indent-1-breaks">&nbsp;&nbsp;&nbsp;&nbsp;</span>
                       <span class="text Matt-5-6"><span class="woj">for they will be filled.<sup class="crossreference" value="(<a href="#cen-NIV-23241E" title="See cross-reference E">E</a>)"></sup></span></span></span><br /><span id="en-NIV-23242" class="text Matt-5-7"><span class="woj"><sup class="versenum">7 </sup>Blessed are the merciful,</span></span><br /><span class="indent-1"><span class="indent-1-breaks">&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="text Matt-5-7"><span class="woj">for they will be shown mercy.<sup class="crossreference" value="(<a href="#cen-NIV-23242F" title="See cross-reference F">F</a>)"></sup></span></span></span><br /><span id="en-NIV-23243" class="text Matt-5-8"><span class="woj"><sup class="versenum">8 </sup>Blessed are the pure in heart,<sup class="crossreference" value="(<a href="#cen-NIV-23243G" title="See cross-reference G">G</a>)"></sup></span></span><br /><span class="indent-1"><span class="indent-1-breaks">&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="text Matt-5-8"><span class="woj">for they will see God.<sup class="crossreference" value="(<a href="#cen-NIV-23243H" title="See cross-reference H">H</a>)"></sup></span></span></span><br /><span id="en-NIV-23244" class="text Matt-5-9"><span class="woj"><sup class="versenum">9 </sup>Blessed are the peacemakers,<sup class="crossreference" value="(<a href="#cen-NIV-23244I" title="See cross-reference I">I</a>)"></sup></span></span><br /><span class="indent-1"><span class="indent-1-breaks">&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="text Matt-5-9"><span class="woj">for they will be called children of God.<sup class="crossreference" value="(<a href="#cen-NIV-23244J" title="See cross-reference J">J</a>)"></sup></span></span></span><br /><span id="en-NIV-23245" class="text Matt-5-10"><span class="woj"><sup class="versenum">10 </sup>Blessed are those who are persecuted because of righteousness,<sup class="crossreference" value="(<a href="#cen-NIV-23245K" title="See cross-reference K">K</a>)"></sup></span></span><br /><span class="indent-1"><span class="indent-1-breaks">&nbsp;&nbsp;&nbsp;&nbsp;</span><span class="text Matt-5-10">
                       <span class="woj">for theirs is the kingdom of heaven.<sup class="crossreference" value="(<a href="#cen-NIV-23245L" title="See cross-reference L">L</a>)"></sup></span></span></span></p></div> <p class="top-05"><span id="en-NIV-23246" class="text Matt-5-11"><span class="woj"><sup class="versenum">11 </sup>“Blessed are you when people insult you,<sup class="crossreference" value="(<a href="#cen-NIV-23246M" title="See cross-reference M">M</a>)"></sup> persecute you and falsely say all kinds of evil against you because of me.<sup class="crossreference" value="(<a href="#cen-NIV-23246N" title="See cross-reference N">N</a>)"></sup></span></span> <span id="en-NIV-23247" class="text Matt-5-12"><span class="woj"><sup class="versenum">12 </sup>Rejoice and be glad,<sup class="crossreference" value="(<a href="#cen-NIV-23247O" title="See cross-reference O">O</a>)"></sup> because great is your reward in heaven, for in the same way they persecuted the prophets who were before you.<sup class="crossreference" value="(<a href="#cen-NIV-23247P" title="See cross-reference P">P</a>)"></sup></span></span></p> <h3><span id="en-NIV-23248" class="text Matt-5-13">Salt and Light</span></h3><p><span class="text Matt-5-13"><span class="woj"><sup class="versenum">13 </sup>“You are the salt of the earth. But if the salt loses its saltiness, how can it be made salty again? It is no longer good for anything, except to be thrown out and trampled underfoot.<sup class="crossreference" value="(<a href="#cen-NIV-23248Q" title="See cross-reference Q">Q</a>)"></sup></span></span></p> <p><span id="en-NIV-23249" class="text Matt-5-14"><span class="woj"><sup class="versenum">14 </sup>“You are the light of the world.<sup class="crossreference" value="(<a href="#cen-NIV-23249R" title="See cross-reference R">R</a>)"></sup> A town built on a hill cannot be hidden.</span></span> <span id="en-NIV-23250" class="text Matt-5-15"><span class="woj"><sup class="versenum">15 </sup>Neither do people light a lamp and put it under a bowl. Instead they put it on its stand, and it gives light to everyone in the house.<sup class="crossreference" value="(<a href="#cen-NIV-23250S" title="See cross-reference S">S</a>)"></sup></span></span>
                       <span id="en-NIV-23251" class="text Matt-5-16"><span class="woj"><sup class="versenum">16 </sup>In the same way, let your light shine before others,<sup class="crossreference" value="(<a href="#cen-NIV-23251T" title="See cross-reference T">T</a>)"></sup> that they may see your good deeds<sup class="crossreference" value="(<a href="#cen-NIV-23251U" title="See cross-reference U">U</a>)"></sup> and glorify<sup class="crossreference" value="(<a href="#cen-NIV-23251V" title="See cross-reference V">V</a>)"></sup> your Father in heaven.</span></span></p> <h3><span id="en-NIV-23252" class="text Matt-5-17">The Fulfillment of the Law</span></h3><p><span class="text Matt-5-17"><span class="woj"><sup class="versenum">17 </sup>“Do not think that I have come to abolish the Law or the Prophets; I have not come to abolish them but to fulfill them.<sup class="crossreference" value="(<a href="#cen-NIV-23252W" title="See cross-reference W">W</a>)"></sup></span></span> <span id="en-NIV-23253" class="text Matt-5-18"><span class="woj"><sup class="versenum">18 </sup>For truly I tell you, until heaven and earth disappear, not the smallest letter, not the least stroke of a pen, will by any means disappear from the Law until everything is accomplished.<sup class="crossreference" value="(<a href="#cen-NIV-23253X" title="See cross-reference X">X</a>)"></sup></span></span> <span id="en-NIV-23254" class="text Matt-5-19"><span class="woj"><sup class="versenum">19 </sup>Therefore anyone who sets aside one of the least of these commands<sup class="crossreference" value="(<a href="#cen-NIV-23254Y" title="See cross-reference Y">Y</a>)"></sup> and teaches others accordingly will be called least in the kingdom of heaven, but whoever practices and teaches these commands will be called great in the kingdom of heaven.</span></span> <span id="en-NIV-23255" class="text Matt-5-20"><span class="woj"><sup class="versenum">20 </sup>For I tell you that unless your righteousness surpasses that of the Pharisees and the teachers of the law, you will certainly not enter the kingdom of heaven.<sup class="crossreference" value="(<a href="#cen-NIV-23255Z" title="See cross-reference Z">Z</a>)"></sup></span></span></p> <h3><span id="en-NIV-23256" class="text Matt-5-21">Murder<sup class="crossreference" value="(<a href="#cen-NIV-23256AA" title="See cross-reference AA">AA</a>)"></sup></span></h3><p><span class="text Matt-5-21"><span class="woj"><sup class="versenum">21 </sup>“You have heard that it was said to the people long ago, ‘You shall not murder,<sup class="footnote" value="[<a href="#fen-NIV-23256a" title="See footnote a">a</a>]">[<a href="#fen-NIV-23256a" title="See footnote a">a</a>]</sup><sup class="crossreference" value="(<a href="#cen-NIV-23256AB" title="See cross-reference AB">AB</a>)">
                       </sup> and anyone who murders will be subject to judgment.’</span></span> <span id="en-NIV-23257" class="text Matt-5-22"><span class="woj"><sup class="versenum">22 </sup>But I tell you that anyone who is angry<sup class="crossreference" value="(<a href="#cen-NIV-23257AC" title="See cross-reference AC">AC</a>)"></sup> with a brother or sister<sup class="footnote" value="[<a href="#fen-NIV-23257b" title="See footnote b">b</a>]">[<a href="#fen-NIV-23257b" title="See footnote b">b</a>]</sup><sup class="footnote" value="[<a href="#fen-NIV-23257c" title="See footnote c">c</a>]">[<a href="#fen-NIV-23257c" title="See footnote c">c</a>]</sup> will be subject to judgment.<sup class="crossreference" value="(<a href="#cen-NIV-23257AD" title="See cross-reference AD">AD</a>)"></sup> Again, anyone who says to a brother or sister, ‘Raca,’<sup class="footnote" value="[<a href="#fen-NIV-23257d" title="See footnote d">d</a>]">[<a href="#fen-NIV-23257d" title="See footnote d">d</a>]</sup> is answerable to the court.<sup class="crossreference" value="(<a href="#cen-NIV-23257AE" title="See cross-reference AE">AE</a>)"></sup> And anyone who says, ‘You fool!’ will be in danger of the fire of hell.<sup class="crossreference" value="(<a href="#cen-NIV-23257AF" title="See cross-reference AF">AF</a>)"></sup></span></span></p> <p><span id="en-NIV-23258" class="text Matt-5-23"><span class="woj"><sup class="versenum">23 </sup>“Therefore, if you are offering your gift at the altar and there remember that your brother or sister has something against you,</span></span> <span id="en-NIV-23259" class="text Matt-5-24"><span class="woj"><sup class="versenum">24 </sup>leave your gift there in front of the altar. First go and be reconciled to them; then come and offer your gift.</span></span></p> <p><span id="en-NIV-23260" class="text Matt-5-25"><span class="woj"><sup class="versenum">25 </sup>“Settle matters quickly with your adversary who is taking you to court. Do it while you are still together on the way, or your adversary may hand you over to the judge, and the judge may hand you over to the officer, and you may be thrown into prison.</span></span> <span id="en-NIV-23261" class="text Matt-5-26"><span class="woj"><sup class="versenum">26 </sup>Truly I tell you, you will not get out until you have paid the last penny.</span></span></p> <h3><span id="en-NIV-23262" class="text Matt-5-27">Adultery</span></h3><p><span class="text Matt-5-27"><span class="woj"><sup class="versenum">27 </sup>“You have heard that it was said, ‘You shall not commit adultery.’<sup class="footnote" value="[<a href="#fen-NIV-23262e" title="See footnote e">e</a>]">[<a href="#fen-NIV-23262e" title="See footnote e">e</a>]</sup><sup class="crossreference" value="(<a href="#cen-NIV-23262AG" title="See cross-reference AG">AG</a>)"></sup></span></span> <span id="en-NIV-23263" class="text Matt-5-28"><span class="woj"><sup class="versenum">28 </sup>But I tell you that anyone who looks at a woman lustfully has already committed adultery with her in his heart.<sup class="crossreference" value="(<a href="#cen-NIV-23263AH" title="See cross-reference AH">AH</a>)"></sup></span></span> <span id="en-NIV-23264" class="text Matt-5-29"><span class="woj"><sup class="versenum">29 </sup>If your right eye causes you to stumble,<sup class="crossreference" value="(<a href="#cen-NIV-23264AI" title="See cross-reference AI">AI</a>)"></sup> gouge it out and throw it away. It is better for you to lose one part of your body than for your whole body to be thrown into hell.</span></span> <span id="en-NIV-23265" class="text Matt-5-30"><span class="woj"><sup class="versenum">30 </sup>And if your right hand causes you to stumble,<sup class="crossreference" value="(<a href="#cen-NIV-23265AJ" title="See cross-reference AJ">AJ</a>)">
                       </sup> cut it off and throw it away. It is better for you to lose one part of your body than for your whole body to go into hell.</span></span></p> <h3><span id="en-NIV-23266" class="text Matt-5-31">Divorce</span></h3><p><span class="text Matt-5-31"><span class="woj"><sup class="versenum">31 </sup>“It has been said, ‘Anyone who divorces his wife must give her a certificate of divorce.’<sup class="footnote" value="[<a href="#fen-NIV-23266f" title="See footnote f">f</a>]">[<a href="#fen-NIV-23266f" title="See footnote f">f</a>]</sup><sup class="crossreference" value="(<a href="#cen-NIV-23266AK" title="See cross-reference AK">AK</a>)"></sup></span></span> <span id="en-NIV-23267" class="text Matt-5-32"><span class="woj"><sup class="versenum">32 </sup>But I tell you that anyone who divorces his wife, except for sexual immorality, makes her the victim of adultery, and anyone who marries a divorced woman commits adultery.<sup class="crossreference" value="(<a href="#cen-NIV-23267AL" title="See cross-reference AL">AL</a>)"></sup></span></span></p> <h3><span id="en-NIV-23268" class="text Matt-5-33">Oaths</span></h3><p><span class="text Matt-5-33"><span class="woj"><sup class="versenum">33 </sup>“Again, you have heard that it was said to the people long ago, ‘Do not break your oath,<sup class="crossreference" value="(<a href="#cen-NIV-23268AM" title="See cross-reference AM">AM</a>)"></sup> but fulfill to the Lord the vows you have made.’<sup class="crossreference" value="(<a href="#cen-NIV-23268AN" title="See cross-reference AN">AN</a>)"></sup></span></span> <span id="en-NIV-23269" class="text Matt-5-34"><span class="woj"><sup class="versenum">34 </sup>But I tell you, do not swear an oath at all:<sup class="crossreference" value="(<a href="#cen-NIV-23269AO" title="See cross-reference AO">AO</a>)"></sup> either by heaven, for it is God’s throne;<sup class="crossreference" value="(<a href="#cen-NIV-23269AP" title="See cross-reference AP">AP</a>)"></sup></span></span> <span id="en-NIV-23270" class="text Matt-5-35"><span class="woj"><sup class="versenum">35 </sup>or by the earth, for it is his footstool; or by Jerusalem, for it is the city of the Great King.<sup class="crossreference" value="(<a href="#cen-NIV-23270AQ" title="See cross-reference AQ">AQ</a>)"></sup></span></span> <span id="en-NIV-23271" class="text Matt-5-36"><span class="woj"><sup class="versenum">36 </sup>And do not swear by your head, for you cannot make even one hair white or black.</span></span> <span id="en-NIV-23272" class="text Matt-5-37"><span class="woj"><sup class="versenum">37 </sup>All you need to say is simply ‘Yes’ or ‘No’;<sup class="crossreference" value="(<a href="#cen-NIV-23272AR" title="See cross-reference AR">AR</a>)">
                       </sup> anything beyond this comes from the evil one.<sup class="footnote" value="[<a href="#fen-NIV-23272g" title="See footnote g">g</a>]">[<a href="#fen-NIV-23272g" title="See footnote g">g</a>]</sup><sup class="crossreference" value="(<a href="#cen-NIV-23272AS" title="See cross-reference AS">AS</a>)"></sup></span></span></p> <h3><span id="en-NIV-23273" class="text Matt-5-38">Eye for Eye</span></h3><p><span class="text Matt-5-38"><span class="woj"><sup class="versenum">38 </sup>“You have heard that it was said, ‘Eye for eye, and tooth for tooth.’<sup class="footnote" value="[<a href="#fen-NIV-23273h" title="See footnote h">h</a>]">[<a href="#fen-NIV-23273h" title="See footnote h">h</a>]</sup><sup class="crossreference" value="(<a href="#cen-NIV-23273AT" title="See cross-reference AT">AT</a>)"></sup></span></span> <span id="en-NIV-23274" class="text Matt-5-39"><span class="woj"><sup class="versenum">39 </sup>But I tell you, do not resist an evil person. If anyone slaps you on the right cheek, turn to them the other cheek also.<sup class="crossreference" value="(<a href="#cen-NIV-23274AU" title="See cross-reference AU">AU</a>)"></sup></span></span> <span id="en-NIV-23275" class="text Matt-5-40"><span class="woj"><sup class="versenum">40 </sup>And if anyone wants to sue you and take your shirt, hand over your coat as well.</span></span> <span id="en-NIV-23276" class="text Matt-5-41"><span class="woj"><sup class="versenum">41 </sup>If anyone forces you to go one mile, go with them two miles.</span></span> <span id="en-NIV-23277" class="text Matt-5-42"><span class="woj"><sup class="versenum">42 </sup>Give to the one who asks you, and do not turn away from the one who wants to borrow from you.<sup class="crossreference" value="(<a href="#cen-NIV-23277AV" title="See cross-reference AV">AV</a>)"></sup></span></span></p> <h3><span id="en-NIV-23278" class="text Matt-5-43">Love for Enemies</span></h3><p><span class="text Matt-5-43"><span class="woj"><sup class="versenum">43 </sup>“You have heard that it was said, ‘Love your neighbor<sup class="footnote" value="[<a href="#fen-NIV-23278i" title="See footnote i">i</a>]">[<a href="#fen-NIV-23278i" title="See footnote i">i</a>]</sup><sup class="crossreference" value="(<a href="#cen-NIV-23278AW" title="See cross-reference AW">AW</a>)"></sup> and hate your enemy.’<sup class="crossreference" value="(<a href="#cen-NIV-23278AX" title="See cross-reference AX">AX</a>)"></sup></span></span> <span id="en-NIV-23279" class="text Matt-5-44"><span class="woj"><sup class="versenum">44 </sup>But I tell you, love your enemies and pray for those who persecute you,<sup class="crossreference" value="(<a href="#cen-NIV-23279AY" title="See cross-reference AY">AY</a>)"></sup></span></span> <span id="en-NIV-23280" class="text Matt-5-45"><span class="woj"><sup class="versenum">45 </sup>that you may be children<sup class="crossreference" value="(<a href="#cen-NIV-23280AZ" title="See cross-reference AZ">AZ</a>)"></sup> of your Father in heaven. He causes his sun to rise on the evil and the good, and sends rain on the righteous and the unrighteous.<sup class="crossreference" value="(<a href="#cen-NIV-23280BA" title="See cross-reference BA">BA</a>)"></sup></span></span> <span id="en-NIV-23281" class="text Matt-5-46"><span class="woj"><sup class="versenum">46 </sup>If you love those who love you, what reward will you get?<sup class="crossreference" value="(<a href="#cen-NIV-23281BB" title="See cross-reference BB">BB</a>)"></sup> Are not even the tax collectors doing that?</span></span> <span id="en-NIV-23282" class="text Matt-5-47"><span class="woj"><sup class="versenum">47 </sup>And if you greet only your own people, what are you doing more than others? Do not even pagans do that?</span></span> <span id="en-NIV-23283" class="text Matt-5-48"><span class="woj"><sup class="versenum">48 </sup>Be perfect, therefore, as your heavenly Father is perfect.<sup class="crossreference" value="(<a href="#cen-NIV-23283BC" title="See cross-reference BC">BC</a>)"></sup></span></span></p><div class="footnotes"><h4>Footnotes:</h4><ol type="a"><li id="fen-NIV-23256a"><a href="#en-NIV-23256" title="Go to Matthew 5:21">Matthew 5:21</a> Exodus 20:13</li>'
            }
        }
    }


//    FastBlur {
//        x: panel.x
//        y: panel.y
//        anchors.fill: root
//        cached: true
//        radius: 32.0
//        source: root
//    }

    Rectangle {
        id: fade
        x: panel.x
        y: panel.y
        width: parent.width
        height: parent.height
        color: '#000000' //root.colorText
        opacity: enabled ? 0.5 : 0;
        enabled: false


        MouseArea {
            anchors.fill: parent
            onClicked: {
                menuFindRect.state = 'off'
                panel.state = 'default'
                parent.enabled = false
            }
        }
    }

    Rectangle {
        id: menuFindRect
        x: panel.x + panel.width - width
        y: panel.y + panel.height
        width: 0
        height: width
        color: panel.color
        state: 'off'

        states: [
            State {
                name: 'on'
            },
            State {
                name: 'off'
            }
        ]

        transitions: [
            Transition {
                from: 'on'
                to: 'off'
                SequentialAnimation {
                    PropertyAction { target: menuClippingsRect; property: 'state'; value: 'off'; }
                    PauseAnimation { duration: root.speedMenu }
                    PropertyAction { target: menuInstallRect; property: 'state'; value: 'off'; }
                    PropertyAction { target: menuOptionsRect; property: 'state'; value: 'off'; }
                    PauseAnimation { duration: root.speedMenu }
                    PropertyAnimation { target: menuFindRect; property: 'width'; to: 0; duration: root.speedMenu; }
                }
                //PropertyAnimation { target: fade; property: 'opacity'; to: 0.0; duration: root.speedMenu * 2 }
            },
            Transition {
                from: 'off'
                to: 'on'
                SequentialAnimation {
                    PropertyAnimation { target: menuFindRect; property: 'width'; to: root.tileWidth; duration: root.speedMenu; }
                    PropertyAction { target: menuInstallRect; property: 'state'; value: 'on'; }
                    PropertyAction { target: menuOptionsRect; property: 'state'; value: 'on'; }
                    PauseAnimation { duration: root.speedMenu }
                    PropertyAction { target: menuClippingsRect; property: 'state'; value: 'on'; }
                }
                //PropertyAnimation { target: fade; property: 'opacity'; to: 0.5; duration: root.speedMenu * 2 }
            }
        ]

        Text {
            text: 'Find'
            color: root.colorBase
            font.pixelSize: parent.width / 5.5
            x: parent.width * 0.1
            y: parent.height * 0.65
            visible: parent.width > 50
        }

        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2.5
            width: parent.width * 0.3
            height: parent.height * 0.3
            radius: width * 0.5
            border.color: root.colorBase
            border.width: 4
            color: "#00000000"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: PropertyAnimation { target: menuFindRect; property: 'color'; to: root.colorText }
            onReleased: PropertyAnimation { target: menuFindRect; property: 'color'; to: root.colorAccent }
            onClicked: { panel.state = 'left'; menuFindRect.state = 'off'; fade.enabled = false }
        }
    }

    Rectangle {
        id: menuInstallRect
        x: menuFindRect.x - width
        y: menuFindRect.y
        width: 0
        height: width
        color: panel.color
        state: 'off'

        states: [
            State {
                name: 'on'
            },
            State {
                name: 'off'
            }
        ]

        transitions: [
            Transition {
                from: 'on'
                to: 'off'
                SequentialAnimation {
                    PropertyAnimation { target: menuInstallRect; property: 'width'; to: 0; duration: root.speedMenu; }
                }
            },
            Transition {
                from: 'off'
                to: 'on'
                SequentialAnimation {
                    PropertyAnimation { target: menuInstallRect; property: 'width'; to: root.tileWidth; duration: root.speedMenu; }
                }
            }
        ]

        Text {
            text: 'Install'
            color: root.colorBase
            font.pixelSize: parent.width / 5.5
            x: parent.width * 0.1
            y: parent.height * 0.65
            visible: parent.width > 50
        }

        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2.5
            width: parent.width * 0.3
            height: parent.height * 0.3
            radius: width * 0.5
            border.color: root.colorBase
            border.width: 4
            color: "#00000000"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: PropertyAnimation { target: menuInstallRect; property: 'color'; to: root.colorText }
            onReleased: PropertyAnimation { target: menuInstallRect; property: 'color'; to: root.colorAccent }
            onClicked: { panel.state = 'left'; menuFindRect.state = 'off'; fade.enabled = false }
        }
    }

    Rectangle {
        id: menuOptionsRect
        x: menuFindRect.x + menuFindRect.width - width
        y: menuFindRect.y + menuFindRect.height
        width: 0
        height: width
        color: panel.color
        state: 'off'

        states: [
            State {
                name: 'on'
            },
            State {
                name: 'off'
            }
        ]

        transitions: [
            Transition {
                from: 'on'
                to: 'off'
                SequentialAnimation {
                    PropertyAnimation { target: menuOptionsRect; property: 'width'; to: 0; duration: root.speedMenu; }
                }
            },
            Transition {
                from: 'off'
                to: 'on'
                SequentialAnimation {
                    PropertyAnimation { target: menuOptionsRect; property: 'width'; to: root.tileWidth; duration: root.speedMenu; }
                }
            }
        ]

        Text {
            text: 'Options'
            color: root.colorBase
            font.pixelSize: parent.width / 5.5
            x: parent.width * 0.1
            y: parent.height * 0.65
            visible: parent.width > 50
        }

        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2.5
            width: parent.width * 0.3
            height: parent.height * 0.3
            radius: width * 0.5
            border.color: root.colorBase
            border.width: 4
            color: "#00000000"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: PropertyAnimation { target: menuOptionsRect; property: 'color'; to: root.colorText }
            onReleased: PropertyAnimation { target: menuOptionsRect; property: 'color'; to: root.colorAccent }
            onClicked: { panel.state = 'left'; menuFindRect.state = 'off'; fade.enabled = false }
        }
    }

    Rectangle {
        id: menuClippingsRect
        x: menuFindRect.x - width
        y: menuFindRect.y + menuFindRect.height
        width: 0
        height: width
        color: panel.color
        state: 'off'

        states: [
            State {
                name: 'on'
            },
            State {
                name: 'off'
            }
        ]

        transitions: [
            Transition {
                from: 'on'
                to: 'off'
                PropertyAnimation { target: menuClippingsRect; property: 'width'; to: 0; duration: root.speedMenu; }
            },
            Transition {
                from: 'off'
                to: 'on'
                PropertyAnimation { target: menuClippingsRect; property: 'width'; to: root.tileWidth; duration: root.speedMenu; }
            }
        ]

        Text {
            text: 'Clippings'
            color: root.colorBase
            font.pixelSize: parent.width / 5.5
            x: parent.width * 0.1
            y: parent.height * 0.65
            visible: parent.width > 50
        }

        Rectangle {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2.5
            width: parent.width * 0.3
            height: parent.height * 0.3
            radius: width * 0.5
            border.color: root.colorBase
            border.width: 4
            color: "#00000000"
        }

        MouseArea {
            anchors.fill: parent
            onPressed: PropertyAnimation { target: menuClippingsRect; property: 'color'; to: root.colorText }
            onReleased: PropertyAnimation { target: menuClippingsRect; property: 'color'; to: root.colorAccent }
            onClicked: { panel.state = 'left'; menuFindRect.state = 'off'; fade.enabled = false }
        }
    }

    Rectangle {
        id: selector
        x: panel.x
        y: panel.y - height
        width: root.width
        height: 600
        color: root.colorAccent

        Flickable {
            anchors.fill: parent
            contentWidth: 1200;
            contentHeight: parent.height

            Text {
                x : 5
                text: 'Favorite:'
                color: root.colorBase
                font.pixelSize: 34
            }

            GridLayout {
                x: 5
                y: 5 + 34 + 5
                columnSpacing: 5
                rowSpacing: 5
                columns: 2

                Rectangle {
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        text: 'KJV'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 5.5
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        visible: parent.width > 50
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: { panel.state = 'default'; fade.enabled = false; }
                    }
                }

                Rectangle {
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth

                    Text {
                        text: 'RusSynodal'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 5.5
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        visible: parent.width > 50
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: { panel.state = 'default'; fade.enabled = false; }
                    }
                }

                Rectangle {
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth

                    Text {
                        text: 'MHC'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 5.5
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        visible: parent.width > 50
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: { panel.state = 'default'; fade.enabled = false; }
                    }
                }

                Rectangle {
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth

                    Text {
                        text: 'ESV'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 5.5
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        visible: parent.width > 50
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: { panel.state = 'default'; fade.enabled = false; }
                    }
                }
            }

            Text {
                x : 5 + 450
                text: 'Bibles:'
                color: root.colorBase
                font.pixelSize: 34
            }

            GridLayout {
                x: 5 + 450
                y: 5 + 34 + 5
                columnSpacing: 5
                rowSpacing: 5
                columns: 2

                Rectangle {
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        text: 'KJV'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 5.5
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        visible: parent.width > 50
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: { panel.state = 'default'; fade.enabled = false; }
                    }
                }

                Rectangle {
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth

                    Text {
                        text: 'RusSynodal'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 5.5
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        visible: parent.width > 50
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: { panel.state = 'default'; fade.enabled = false; }
                    }
                }

                Rectangle {
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth

                    Text {
                        text: 'ESV'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 5.5
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        visible: parent.width > 50
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: { panel.state = 'default'; fade.enabled = false; }
                    }
                }
            }

            Text {
                x : 5 + 450 + 450
                text: 'Commentaries:'
                color: root.colorBase
                font.pixelSize: 34
            }

            GridLayout {
                x: 5 + 450 + 450
                y: 5 + 34 + 5
                columnSpacing: 5
                rowSpacing: 5
                columns: 2

                Rectangle {
                    color: root.colorBase
                    width: root.tileWidth
                    height: root.tileWidth


                    Text {
                        text: 'MHC'
                        color: root.colorAccent
                        font.pixelSize: parent.width / 5.5
                        x: parent.width * 0.1
                        y: parent.height * 0.65
                        visible: parent.width > 50
                    }

                    Rectangle {
                        x: (parent.width - width) / 2
                        y: (parent.height - height) / 2.5
                        width: parent.width * 0.3
                        height: parent.height * 0.3
                        radius: width * 0.5
                        border.color: root.colorAccent
                        border.width: 4
                        color: "#00000000"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: { panel.state = 'default'; fade.enabled = false; }
                    }
                }
            }
        }
    }

    Rectangle {
        x: panel.x + root.width
        color: root.colorBase
        width: root.width
        height: root.height
        id: findView

        Rectangle {
            x: parent.x
            y: parent.y
            height: panel.height
            width: parent.width
            color: root.colorAccent

            Rectangle {
                anchors.fill: parent
                anchors.right: parent.width - panel.height
                anchors.margins: 5
                color: root.colorBase
                y: 10
                x: 5

                TextInput {
                    anchors.fill: parent
                    font.pixelSize: 34
                }
            }

            Rectangle {
                id: findControl
                anchors.fill: parent
                anchors.leftMargin: parent.width - panel.height
                color: parent.color

                Rectangle {
                    width: parent.width * 0.8
                    height: parent.height * 0.8
                    anchors.centerIn: parent
                    radius: width * 0.5
                    border.color: root.colorBase
                    border.width: 4
                    color: "#00000000"
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: PropertyAnimation { target: findControl; property: 'color'; to: root.colorText }
                    onReleased: PropertyAnimation { target: findControl; property: 'color'; to: root.colorAccent }
                    onClicked: panel.state = 'default'
                }
            }
        }
    }
}
