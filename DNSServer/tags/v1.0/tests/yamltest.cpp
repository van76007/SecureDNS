#include <stdio.h>
#include <yaml.h>
#include <string>
#include <string.h>
#include <iostream>


int main(void)
{
 FILE *FileHandler = fopen("public.yaml", "r");
  yaml_parser_t Yamlparser;
  yaml_event_t  Yamlevent;   /* New variable */

  /* Initialize parser */
  if(!yaml_parser_initialize(&Yamlparser))
    fputs("Failed to initialize parser!\n", stderr);
  if( NULL == FileHandler)
    fputs("Failed to open file!\n", stderr);

  yaml_parser_set_input_file(&Yamlparser, FileHandler);
	std::string strSuffix[105];
	int x = 0;
	std::string PortNums;
	std::string NameserverInfo;
	std::string DatabaseInfo;
  
  do {
    yaml_parser_parse(&Yamlparser, &Yamlevent);

	if(Yamlevent.type == YAML_SCALAR_EVENT){
		//printf("actual: %s\n", event.data.scalar.value);
		std::string actual((char*)Yamlevent.data.scalar.value);
		//determine the list of ports
		std::string Portlist = "ListenPort";
		if( strcmp( actual.c_str(), Portlist.c_str() ) ) {
		}else{
			if(Yamlevent.type != YAML_STREAM_END_EVENT)
 			     yaml_event_delete(&Yamlevent);	
 			yaml_parser_parse(&Yamlparser, &Yamlevent);
			 PortNums = (char*)Yamlevent.data.scalar.value; 
			
		}
	
		//determine the Nameserver Info
		std::string Nameserver = "Nameserver";
		if( strcmp( actual.c_str(), Nameserver.c_str() ) ) {
		}else{
			if(Yamlevent.type != YAML_STREAM_END_EVENT)
 			     yaml_event_delete(&Yamlevent);	
 			yaml_parser_parse(&Yamlparser, &Yamlevent);
			NameserverInfo = (char*)Yamlevent.data.scalar.value; 
		}

		//determine the Info for the Database
		std::string Database = "Database";
		if( strcmp( actual.c_str(), Database.c_str() ) ) {
		}else{
			if(Yamlevent.type != YAML_STREAM_END_EVENT)
 			     yaml_event_delete(&Yamlevent);	
 			yaml_parser_parse(&Yamlparser, &Yamlevent);
			DatabaseInfo = (char*)Yamlevent.data.scalar.value; 
		}	
    }
    
	x++;
  } while(Yamlevent.type != YAML_STREAM_END_EVENT);

	std::cout << "Ports: "  << PortNums << std::endl;
	std::cout << "NameserverInfo: " << NameserverInfo << std::endl;
	std::cout << "DatabaseInfo: " << DatabaseInfo << std::endl;

  yaml_event_delete(&Yamlevent);  
  yaml_parser_delete(&Yamlparser);
  fclose(FileHandler);
  return 0;
}

