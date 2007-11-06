#!/usr/bin/env kross

"""
Kopete python script to test some scripting functionality.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import Kopete

# This function got called if the plugin got started.
def pluginInit():
    print "===============> pluginInit"
    iface = Kopete.interface()
    print "accounts=%s" % iface.accounts()
    print "contacts=%s" % iface.contacts()
    print "identities=%s" % iface.identities()
    #iface.setStatusMessage("Some statusmessage")

    Kopete.addCommand("myscript", "USAGE: /myscript- Does something", 0, -1, "python")
    Kopete.addCommand("myotherscript", "USAGE: /myotherscript - Does something", 0, -1, "python")

# This function got called if the plugin is going to be shutdown.
def pluginFinish():
    print "===============> pluginFinish"
    #Kopete.interface().setStatusMessage("Another statusmessage")

# This function got called if we received a new message.
def messageReceived(message):
    print "===============> messageReceived subject=%s message=%s" % (message.subject(), message.plainBody())
    message.setPlainBody("RECEIVED %s" % message.plainBody())
    chats = Kopete.chats()
    print "%s" % chats
    for c in chats:
        print c.displayName()

# This function got called if we sent a new message.
def messageSent(message):
    print "!===============> messageSend subject=%s message=%s" % (message.subject(), message.plainBody())
    message.setPlainBody("SEND %s" % message.plainBody())

# This function got called if a new chatsession opened.
def chatAdded(chat):
    print "!===============> chatAdded chat=%s" % chat

# This function got called if a opened chatsession got closed.
def chatRemoved(chat):
    print "!===============> chatRemoved chat=%s" % chat
    
# This function got called if a command got executed.
def commandExecuted(command, chat):
    print "===============> commandExecuted command=%s chat=%s" % (command,chat)

# This function got called if settings changed.
def settingsChanged():
    print "===============> settingsChanged"

print "Testcase.py loaded"
