#!/usr/bin/env kross

"""
Kopete python script to test Kopete scripting functionality.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
Dual-licensed under LGPL v2+higher and the BSD license.
"""

import Kross, Kopete

# This function got called if the plugin got started.
def pluginInit():
    print "pluginInit"
    iface = Kopete.interface()
    print "accounts=%s" % iface.accounts()
    print "contacts=%s" % iface.contacts()
    print "identities=%s" % iface.identities()
    for i in iface.identities():
        print "identity=%s label=%s" % (i,iface.labelForIdentity(i))
    for p in iface.protocols():
        print "protocol=%s" % p
    
    iface.setStatusMessage("Some statusmessage")

    # register a custom command.
    Kopete.addCommand("mycommand", "USAGE: /mycommand - This is a scripted command", 0, -1, "python")
    # register a custom action.
    Kopete.addContactAction("mycontactaction", "My Contact Script Action", "python")
    
    print "-------------------------------------------------------------------------------------------"
    print iface.isConnected("Testbed","MyAccount")
    iface.connect("Testbed","MyAccount")
    #for a in Kopete.accounts():
        #print "accountId=%s isConnected=%s" % (a.accountId,a.isConnected)
        #if a.accountId == "MyAccount":
            #a.connect()

# This function got called if the plugin is going to be shutdown.
def pluginFinish():
    print "pluginFinish"
    #Kopete.interface().setStatusMessage("Another statusmessage")

# This function got called if we received a new message.
def messageReceived(message):
    print "messageReceived subject=%s message=%s" % (message.subject(), message.plainBody())
    message.setPlainBody("RECEIVED %s" % message.plainBody())
    chats = Kopete.chats()
    print "%s" % chats
    for c in chats:
        print c.displayName()

# This function got called if we sent a new message.
def messageSent(message):
    print "messageSend subject=%s message=%s" % (message.subject(), message.plainBody())
    message.setPlainBody("SEND %s" % message.plainBody())

# This function got called if a new chatsession opened.
def chatAdded(chat):
    print "chatAdded chat=%s" % chat
    
    # register a custom chat action.
    chat.addAction("mychataction", "My Chat Script Action", "python")

# This function got called if an opened chatsession got closed.
def chatRemoved(chat):
    print "chatRemoved chat=%s" % chat
    
# This function got called if a custom command got executed.
def commandExecuted(chat, name, args):
    chat.appendMessage("Command <b>%s</b> executed with args %s" % (name,args))
    if name == "mycommand":
        chat.sendMessage("Send something for command <b>%s</b>" % name)

# This function got called if a custom chat-action got executed.
def chatActionExecuted(chat, name):
    chat.appendMessage("Action <b>%s</b> executed" % name)
    if name == "mychataction":
        chat.sendMessage("Send something for action <b>%s</b>" % name)

# This function got called if a custom contact-action got executed.
def contactActionExecuted(contact, name):
    print "contactActionExecuted contact=%s name=%s" % (contact,name)
    if name == "mycontactaction":
        contact.execute()

print "Testcase.py loaded"
