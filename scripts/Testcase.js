/*
Kopete python script to test Kopete scripting functionality.

(C)2007 Sebastian Sauer <mail@dipe.org>
http://kross.dipe.org
Dual-licensed under LGPL v2+higher and the BSD license.
*/

// This function got called if the plugin got started.
function pluginInit()
{
    println("pluginInit");

    // register a custom command.
    Kopete.addCommand("mycommand", "USAGE: /mycommand - This is a scripted command", 0, -1, "script");
    // register a custom action.
    Kopete.addContactAction("kjscontactaction", "Kjs Contact Action", "script");
}

/// This function got called if the plugin is going to be shutdown.
function pluginFinish()
{
    println("pluginFinish");
}

// This function got called if we received a new message.
function messageReceived(message)
{
    println("messageReceived subject=" + message.subject() + " message=" + message.plainBody());
}

// This function got called if we sent a new message.
function messageSent(message)
{
    println("messageReceived subject=" + message.subject() + " message=" + message.plainBody());
}

// This function got called if a new chatsession opened.
function chatAdded(chat)
{
    println("chatAdded displayName=" + chat.displayName());

    // register a custom chat action.
    chat.addAction("kjschataction", "Kjs Chat Action", "script");
}

// This function got called if an opened chatsession got closed.
function chatRemoved(chat)
{
    println("chatRemoved displayName=" + chat.displayName());
}
   
// This function got called if a custom command got executed.
function commandExecuted(chat, commandname, args)
{
    println("commandExecuted commandname=" + commandname + " args=" + args);
}

// This function got called if a custom chat-action got executed.
function chatActionExecuted(chat, actionname)
{
    println("chatActionExecuted actionname=" + actionname);
}

// This function got called if a custom contact-action got executed.
function contactActionExecuted(contact, actionname)
{
    println("contactActionExecuted contact=" + contact.formattedName + " actionname=" + actionname);
}

println("Testcase.js loaded");
