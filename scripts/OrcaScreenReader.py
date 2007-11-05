#!/usr/bin/env kross

"""
Kopete python script to let the Orca Screenreader talk.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import sys, os, urllib, Kross, Kopete

class MyConfig:
    def __init__(self):
        self.url = "http://127.0.0.1:20433"
        self.speakReceived = True
        self.speakSent = False

class MyOrca:
    def __init__(self, config):
        self.config = config
        self.isConnected = False
    def _send(self, data):
        f = urllib.urlopen(self.config.url, data)
        s = f.read()
        f.close()
        return s
    def speak(self, text):
        self._send("speak:%s" % text)
    def stop(self):
        self._send("stop")
    def isSpeaking(self):
        return self._send("isSpeaking")

myconfig = MyConfig()
myorca = MyOrca(myconfig)

def pluginInit():
    global myorca, myconfig
    while True:
        try:
            myorca.speak("Kopete Instant Messenger")
            myorca.isConnected = True
            break
        except IOError:
            forms = Kross.module("forms")
            dialog = forms.createDialog("Orca Screen Reader")
            dialog.minimumWidth = 400
            dialog.minimumHeight = 40
            dialog.setButtons("Ok|Cancel")
            page = dialog.addPage("","")
            label = forms.createWidget(page, "QLabel")
            label.text = "Failed to connect with the Orca HTTP-Server."
            widget = forms.createWidget(page, "QWidget")
            layout = forms.createLayout(widget, "QHBoxLayout")
            forms.createWidget(widget, "QLabel").text = "Url:"
            urlEdit = forms.createWidget(widget, "QLineEdit")
            urlEdit.text = myconfig.url
            if not dialog.exec_loop():
                return
            myconfig.url = urlEdit.text

def pluginFinish():
    global myorca, myconfig
    if myorca.isConnected:
        myorca.isConnected = False

def messageReceived(message):
    global myorca, myconfig
    if myconfig.speakReceived:
        #contact = message.sender()
        #print contact
        #name = contact.nickName
        #if not name:
            #name = message.formattedName
        myorca.speak( message.plainBody() )

def messageSent(message):
    global myorca, myconfig
    if myconfig.speakSent:
        myorca.speak( message.plainBody() )

print "OrcaScreenReader.py loaded"
