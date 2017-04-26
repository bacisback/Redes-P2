#include "UsoSockets.h"
//#include "conexion.h"
void (* pcommandUserCliente[numActions])(char *)={names,funcDef,list,join,part,funcDef,quit,nick,away,whois, funcDef, kick,topic,funcDef,msgpriv};


void Analize(long response, char* string){
	if(getFlag() == 0) return;
	if(string[0] == '/')
		(*pcommandUserCliente[response -1])(string);
	else{//se trata de un mensaje privado
		char* canal = IRCInterface_ActiveChannelName ();
		char* command,*nick;
		IRCMsg_Privmsg (&command, NULL, canal, string);
		send(getsocketTCP(), command, strlen(command),0);
		IRCInterface_PlaneRegisterInMessage (command);
		getNick(&nick);
		IRCInterface_WriteChannel(canal, nick, string);
		IRC_MFree(3,&canal,&nick,&command);
		
	}
	return;
}
void funcDef(char * str){
	
	printf("Este comando de usuario no está implementado");
	
}
void names(char * str){

	char * channel, * targetserver ,*command;

	IRCUserParse_Names( str, &channel, &targetserver);
	IRCMsg_Names (&command, NULL, channel, targetserver);

	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage (command);

	free(targetserver);
	free(command);
	free(channel);

}

void list (char * str){

	char * channel, * target, *command;

	IRCUserParse_List(str, &channel, &target);
	IRCMsg_List(&command, NULL, channel, target);
	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage(command);

	free(channel);
	free(target);
	free(command);
}

void join(char * str){

	char * channel, * password, *command,*server;
	//real y host incluirlos en usuario.c
	
	getServer(&server);
	IRCUserParse_Join( str, &channel, &password);

	IRCMsg_Join (&command, NULL, channel, password, NULL);
	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage(command);
	free(command);
	IRCMsg_Who (&command, NULL, channel, NULL);
	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage(command);
	/*free(command);
	IRCMsg_Topic(&command, NULL, channel, NULL);
	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterOutMessage(command);*/
	IRC_MFree(4,&server,&command,&channel,&password);
	return;
	/*
	IRCInterface_ListAllChannelsThread (&name, &num);
	
	getNick(&nick);
	getUser(&nuser);
	getRealName(&real);

	for(i =0; i<num; i++){
		//si el usuario se quiere añadir a un canal ya creado
		if(strcmp(name[i],channel)==0){
			IRCInterface_AddNickChannelThread (channel, nick,
			 nuser, real, host, 0);
			flag=1;
		}
	}
	//no existe ningun canal con ese nombre, por lo que se crea otro nuevo
	if(flag==0){
		long mode;
		mode = IRCInterface_ModeToIntMode("+o");	
		printf("%s\n",command);	
		IRCInterface_AddNewChannelThread(channel, mode);
		IRCInterface_AddNickChannelThread (channel, nick,
			 nuser, real, getServer(), OPERATOR);
	}
	
	
	IRCInterface_FreeListAllChannelsThread (name, num);
	IRC_MFree(7,&nuser,&host,&real,&channel,&password, &command, &nick);*/
}

void part(char * str){

	char  * msg, *command, * channel;
	IRCUserParse_Part (str, &msg);

	channel = IRCInterface_ActiveChannelName();

	IRCMsg_Part (&command, NULL, channel, msg);
	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage (command);
	//IRCInterface_RemoveAllNicksChannelThread(channel);
	
	free(msg);
	free(command);
}

void quit(char * str){

	char * reason, *command;

	IRCUserParse_Quit (str, &reason);
	IRCMsg_Quit (&command, NULL, reason);

	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage (command);
	IRCInterface_WriteSystem ("*", "Desconectado()");
	IRC_MFree(2,&reason,&command);
	FreeUser();
}

void nick(char * str){

	char * newnick, *command;
	

	IRCUserParse_Nick (str, &newnick);
	
	
	IRCMsg_Nick (&command, NULL,newnick , NULL);
	
	
	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage (command);
	
	free(newnick);
	
	free(command);

}
void away(char * str){
	
	char * reason, *command;

	IRCUserParse_Away(str, &reason);
	
	if(reason == NULL)
		reason=strdup("Estoy ocupado");

	IRCMsg_Away (&command, NULL, reason);

	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage (command);
	
	free(reason);
	free(command);

}

void whois(char * str){

	char * command, *mask;

	IRCUserParse_Whois (str, &mask);

	IRCMsg_Whois(&command, NULL, NULL, mask);
	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage (command);
	
	
	free(mask);
	free(command);
}

void kick(char * str){

	

}
void topic(char * str){

	char * command, *topic, *channel;

	IRCUserParse_Topic (str, &topic);

	channel = IRCInterface_ActiveChannelName();
	IRCMsg_Topic(&command, NULL, channel, topic);
	
	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage (command);
	free(topic);
	free(command);
	

}
void msgpriv(char * str){

	char * command, *msg, *nickorchannel, *nick, *nuser, *real, *host;

	IRCUserParse_Msg (str, &nickorchannel, &msg);

	IRCMsg_Privmsg (&command, NULL, nickorchannel, msg);
	
	send(getsocketTCP(), command, strlen(command),0);
	IRCInterface_PlaneRegisterInMessage (command);
	getNick(&nick);
	getUser(&nuser);
	getRealName(&real);
	getServer(&host);
	if(IRCInterface_QueryChannelExist(nickorchannel) != TRUE){
		IRCInterface_AddNewChannel(nickorchannel, 0);
		IRCInterface_AddNickChannel (nickorchannel , nick, nuser, real, host, 2);
		IRCInterface_AddNickChannel (nickorchannel , nickorchannel, nickorchannel, nickorchannel, host, 2);
	}
	IRCInterface_WriteChannel(nickorchannel, nick, msg);
	
	free(nick);
	free(msg);
	free(command);
	free(nickorchannel);

}























































