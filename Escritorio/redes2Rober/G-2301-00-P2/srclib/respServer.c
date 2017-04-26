#include "UsoSockets.h"

void (* pcommandRespServer[numResp])(char *)={rplAway,funcDefServer,funcDefServer,funcDefServer,funcDefServer,rplNowAway};


void AnalizeServer(long response, char* buf){
	IRCInterface_PlaneRegisterOutMessageThread (buf);
	//if(response >301)
		//(* pcommandRespServer [response -301])(buf);
	switch(response){
		case 7:
			RemoveNick(buf);
		case 9:
			ServerJoin(buf);
			break;
		case 16:
			ServerPrivMsg(buf);
		case 34:
			ServerPing(buf);
			break;
		case 83:
			ServerWhoIsUser(buf);
			break;
		case 86:
			ServerWhoIsServer(buf);
			break;
		case 87:
			ServerWhoIsIdle(buf);
			break;
		case 88:
			ServerWho(buf);
			break;
		case 78:
			ServerEndWho(buf);
			break;
		case 84:
			ServerWhoIsChannels(buf);
			break;
		case 113:
			ServerNames(buf);
			break;
		case 117:
			ServerNoTopic(buf);
			break;
		case 124:
			ServerReplyTopic(buf);
			break;
		case 201:
			ServerTopicSetter(buf);
			break;//rply de quien ha puesto el topic
		case 182:
			ServerWhoIs(buf);
			break;
		case 79:
			ServerEndWhoIs(buf);
			break;
		default:
			funcDefServer(buf);
	}
}
void RemoveNick(char* str){
	char *prefix,*msg,**channels,*token;
	int num,i;
	token = strtok(str, "!");
	token ++;
	if(IRCParse_Quit (str, &prefix, &msg) != IRC_OK)
		return;
	IRCInterface_ListAllChannelsThread (&channels, &num);
	for(i = 0; i < num; i++)
		IRCInterface_DeleteNickChannelThread (channels[i], token);
	for(i = 0; i < num; i++)
		free(channels[i]);
	free(msg);
	return;
	
}
void ServerNoTopic(char* str){
	char *prefix,*nick,*channel,*topic,NoTopic[500];
	if(IRCParse_RplNoTopic (str, &prefix, &nick, &channel, &topic)!= IRC_OK)
		return;
	printf("%s\n",channel);
	strcat(NoTopic, channel);
	strcat(NoTopic, "No topic is set");
	IRCInterface_WriteChannelThread (channel, "*", NoTopic);
	IRC_MFree(4,&prefix, &nick, &channel, &topic);
	return;
}
void ServerPrivMsg(char * str){
	char *prefix,*msgtarget,*msg,*token,*nick,*nuser,*real,*host,*nick2,*user2;
	if(IRCParse_Privmsg (str, &prefix, &msgtarget, &msg)!= IRC_OK)
		return;
	token = strtok(str, "!");
	token ++;
	if(msgtarget[0] != '#'){
		if(IRCInterface_QueryChannelExistThread (msgtarget) != TRUE){
			char *token2, *token3, *canal;
			getNick(&nick);
			getUser(&nuser);
			getRealName(&real);
			getServer(&host);
			nick2 = strdup(token);
			if(strcmp(msgtarget,nick) != 0){ // Vemos si el mensaje es enviado por nosotros o a nosotros
				IRCInterface_AddNewChannelThread(msgtarget, 0);
				canal = strdup(msgtarget);	// El nombre del nuevo canal es del usuario a quien vas
			}else{
				IRCInterface_AddNewChannelThread(nick2, 0);
				canal = strdup(nick2);
			}
			
			token = strtok(NULL,"!");
			token2 = strtok(token, "@");
			token2++;
			user2 = strdup(token2);
			token3 = strtok(token2, " ");
			IRCInterface_AddNickChannelThread (canal , nick, nuser, real, host, 2);
			IRCInterface_AddNickChannelThread (canal , nick2, user2, real, token3, 2);
			free(msgtarget);
			msgtarget=strdup(canal);
		}
	}
	if(strcmp(msg, "\001AUDIOCHAT Petition") == 0){//AUDIO 
            char audiomsg[100];
		int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		setSocketUDP(sockfd);
		listen(sockfd, 1); // se pone a la escucha el socket, máximo 1 cliente
		struct sockaddr_in my_addr;
		socklen_t slen = sizeof(my_addr);
		getsockname(sockfd, (struct sockaddr*)&my_addr, &slen);
		struct hostent *Maquina;
		Maquina = gethostbyname ("localhost");
		char *buf;
		sprintf(audiomsg,"\\001AUDIOCHAT %s %d",inet_ntoa(* ((struct in_addr *)Maquina->h_addr)), ntohs(my_addr.sin_port));
		
		IRCMsg_Privmsg (&buf, NULL, msgtarget, msg);
		
		printf("%s\n",buf);
		send(getsocketTCP(),buf,strlen(buf),0);	
		IRC_MFree(4, &buf, &prefix, &msgtarget, &msg);
		return;
	}
	IRCInterface_WriteChannelThread (msgtarget, token, msg);
	IRC_MFree(3, &prefix, &msgtarget, &msg);
	return;
}
	
void ServerEndWho(char *str){
	return;
}
void ServerReplyTopic(char *str){
	char *prefix,*nick,*channel,*topic;
	if(IRCParse_RplTopic (str, &prefix, &nick, &channel, &topic)!= IRC_OK)
		return;
	IRCInterface_SetTopic(topic);
	IRCInterface_WriteChannelThread (channel, "*", str);
	IRC_MFree(4,&prefix, &nick, &channel, &topic);
	return;
}
void ServerTopicSetter(char *str){
	char *prefix,*nick,*channel,*topic;
	if(IRCParse_RplTopic (str, &prefix, &nick, &channel, &topic)!= IRC_OK)
		return;
	IRCInterface_WriteChannelThread (channel, "*", str);
	IRC_MFree(4,&prefix, &nick, &channel, &topic);
}
void ServerWho(char *str){
	char* prefix,*nick,*channel,*user,*host,*server,*nick2,
		*type,*msg,*realname;
	int hopcount;
	if(IRCParse_RplWhoReply (str, &prefix, &nick, &channel,
							&user, &host, &server, &nick2, 
							&type,&msg,&hopcount, &realname)!= IRC_OK)
		return;
	IRCInterface_AddNickChannelThread (channel, nick2, user, realname, host, 0);
	IRC_MFree(10,&prefix, &nick, &channel,&user, &host, &server, &nick2, 
							&type,&msg, &realname);
	return;
}
void ServerWhoIsServer(char*str){
	char * prefix,*nick,*nick2,*server,*serverinfo;
	if( IRCParse_RplWhoIsServer (str, &prefix, &nick, &nick2, &server, &serverinfo) != IRC_OK)
		return;
	IRCInterface_WriteSystemThread ("*", str);
	IRC_MFree(5, &prefix, &nick, &nick2, &server, &serverinfo);
}
void ServerWhoIsChannels(char* str){
	char *prefix,*nick,*nick2,*chanelstr;	
	if(IRCParse_RplWhoIsChannels (str,&prefix, &nick, &nick2, &chanelstr) !=IRC_OK)
		return;
	IRCInterface_WriteSystemThread ("*", str);
	IRC_MFree(4,&prefix, &nick, &nick2, &chanelstr);
}
void ServerWhoIsIdle(char *str){
	char *prefix,*nick,*nick2,*msg,*timeElapse,resp[500];
	int secs_idle,signon;
	
	if(IRCParse_RplWhoIsIdle (str, &prefix, &nick, &nick2, &secs_idle, &signon, &msg)!=IRC_OK)
		return;
	timeElapse=IRCTAD_TimestampToLocalDate (signon);
	sprintf(resp,"[%s] inactividad %d (seconds idle), entró el: %s\n",nick,secs_idle,timeElapse);
	IRCInterface_WriteSystemThread ("*", resp);
	IRC_MFree(5,&prefix, &nick, &nick2,  &msg, &timeElapse);
}
void ServerPing(char * str){
	char*prefix = NULL,*server = NULL,*server2  = NULL,*msg = NULL,command[100];
	if(IRCParse_Ping (str, &prefix, &server, &server2, &msg)!=IRC_OK)
		return;
	
	getServer(&server2);
	if(server){
		server ++;
		sprintf(command,"PONG %s %s", server2,server); 
	}
	else
		sprintf(command,"PONG"); 
	
	printf("%s\n",command);fflush(stdout);
	
	send(getsocketTCP(),command,strlen(command),0);
	//IRC_MFree(4,&prefix, &server, &server2, &msg);
	printf("Lo hise\n");fflush(stdout);
	return;
}
void funcDefServer(char * str){
	IRCInterface_WriteSystemThread ("ERR comando no conocido", str);
	return;
}
void ServerWhoIsUser(char* str){
	char* prefix,*nick,*nick2,*name,*host,*real;
	if(IRCParse_RplWhoIsUser (str, &prefix, &nick, &nick2, &name, &host, &real) != IRC_OK)
		return;  
	IRCInterface_WriteSystemThread ("*", str);	
	IRC_MFree(6, &prefix, &nick, &nick2, &name, &host, &real);
}
void ServerWhoIs(char * str){
	char* prefix,*target,*mask;
	if(IRCParse_Whois (str, &prefix, &target, &mask)!= IRC_OK)
		return;
	
	IRCInterface_WriteSystemThread ("*", str);
	IRC_MFree(3,&prefix,&mask,&target);
	return;	
}
void ServerEndWhoIs(char * str){
	
	char* prefix,*nick,*name,*msg;
	if(IRCParse_RplEndOfWhoIs (str, &prefix, &nick, &name, &msg)!= IRC_OK)
		return;
	
	IRCInterface_WriteSystemThread ("*", msg);
	IRC_MFree(3,&prefix,&nick,&name,&msg);
	return;	
}
	
void ServerJoin(char * str){
	
	char * channel, * password, *command, *nick,*nuser, *real, *host, *prefix,*msg,*token;
	
	if(IRCParse_Join(str,&prefix,&msg, &password,&channel) != IRC_OK)
		return;
	
	token = strtok(str, "!");
	token ++;
	getNick(&nick);
	if(strcmp(nick,token) == 0){
		getUser(&nuser);
		getRealName(&real);
		getServer(&host);
		if(IRCInterface_QueryChannelExistThread (channel) == TRUE){
			IRCInterface_AddNickChannelThread (channel, nick, nuser, real, host, 0);
		}
		//no existe ningun canal con ese nombre, por lo que se crea otro nuevo
		else{	
			IRCInterface_AddNewChannelThread(channel, 0);
			IRCInterface_AddNickChannelThread (channel, nick, nuser, real, host, 2);
		}
		free(nuser);free(real);free(host);
	}else{
		IRCMsg_Who (&command, NULL, channel, NULL);
		send(getsocketTCP(), command, strlen(command),0);
		IRCInterface_PlaneRegisterOutMessage(command);
		free(command);
	}
	IRC_MFree(4,&prefix,&password, &nick,&msg);
		
}
void ServerNames(char * str){
	char *prefix,*channel,*target;
	if(IRCParse_Names (str, &prefix, &channel, &target) != IRC_OK)
		return;
	IRCInterface_WriteSystemThread ("*", str);	
	IRC_MFree(3,&prefix,&channel,&target);
	return;


}
void rplWelcome(char * str){
	
	char * prefix, *nick, * msg;
	
	IRCParse_RplWelcome (str, &prefix, &nick, &msg);
	
	/*es posible que no sea necesario ni parsear el welcome puesto que si se recibe esto indica que el usuario ya se ha registrado correctamente*/
	/*tal vez habria que crear el usuario aqui */
	
	free(prefix);
	free(nick);
	free(msg);
}
void rplAway(char * str){
	
	char * prefix, *nick, *nick2, * msg, *command;
	IRCParse_RplAway (str, &prefix, &nick, &nick2, &msg);
	
	IRCMsg_Away (&command, nick2, msg);
	
	IRCInterface_WriteSystemThread (nick, msg);
	
	free(command);
	free(nick);
	free(nick2);
	free(msg);
	free(prefix);
	
}
void rplNowAway(char * str){
	
	//llamar a la funcion que ponga el nombre del cliente en un tono mas claro
	
}
void respNotice(char * str){
	
	char * prefix, *msgtarget, *msg, *nick;
	
	IRCParse_Notice (str, &prefix, &msgtarget, &msg);
	getNick(&nick);
	IRCInterface_WriteSystemThread(nick, msg);
	
	IRC_MFree(4, &prefix, &msgtarget, &msg, &nick);
}


