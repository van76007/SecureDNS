#include <stdio.h>
#include <yaml.h>
#include <string>
#include <string.h>
#include <iostream>


int main(void)
{
  FILE *fh = fopen("public.yaml", "r");
  yaml_parser_t parser;
  yaml_event_t  event;   /* New variable */

  /* Initialize parser */
  if(!yaml_parser_initialize(&parser))
    fputs("Failed to initialize parser!\n", stderr);
  if(fh == NULL)
    fputs("Failed to open file!\n", stderr);

  /* Set input file */
  yaml_parser_set_input_file(&parser, fh);
	std::string strSuffix[105];
	int x = 0;
  /* START new code */
  do {
	
    yaml_parser_parse(&parser, &event);
    switch(event.type)
    { 
	case YAML_NO_EVENT: puts("No event!"); break;
	/* Stream start/end */
	case YAML_STREAM_START_EVENT: puts("STREAM START"); break;
	case YAML_STREAM_END_EVENT:   puts("STREAM END");   break;
	/* Block delimeters */
	case YAML_DOCUMENT_START_EVENT: puts("<b>Start Document</b>"); break;
	case YAML_DOCUMENT_END_EVENT:   puts("<b>End Document</b>");   break;
	case YAML_SEQUENCE_START_EVENT: puts("<b>Start Sequence</b>"); break;
	case YAML_SEQUENCE_END_EVENT:   puts("<b>End Sequence</b>");   break;
	case YAML_MAPPING_START_EVENT:  puts("<b>Start Mapping</b>");  break;
	case YAML_MAPPING_END_EVENT:    puts("<b>End Mapping</b>");    break;
	/* Data */
	case YAML_ALIAS_EVENT:  printf("Got alias (anchor %s)\n", event.data.alias.anchor); break;
	case YAML_SCALAR_EVENT: printf("Got scalar (value %s)\n", event.data.scalar.value); 
		std::string actual((char*)event.data.scalar.value);
std::string he = "ListenPort";
		if( strcmp( actual.c_str(), he.c_str() ) ) {
			
		}	else{
			printf("Heelothere\n");
		}
		//printf("actual: %s\n", event.data.scalar.value);
	break;
	
    }
	
 //	char* actual = (char*)event.data.scalar.value;
	
//lstd::cout << "event.data.scalar.value: " << event.data.scalar.value << std::endl;
   /*    if( strcmp( actual.c_str(), he.c_str() ) ) {

		printf("Heelothere\n");
	}
*/
    if(event.type != YAML_STREAM_END_EVENT)
      yaml_event_delete(&event);
	x++;
  } while(event.type != YAML_STREAM_END_EVENT);
  yaml_event_delete(&event);
  /* END new code */
  /* Cleanup */
  yaml_parser_delete(&parser);
  fclose(fh);
  return 0;
}

