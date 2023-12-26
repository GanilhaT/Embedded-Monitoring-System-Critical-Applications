# Embedded Monitoring System for Critical Applications

Repository for the code developed in the context of my thesis entitled "Monitorização de sistemas embebidos para aplicações críticas" for the master's degree in Computer Science and Engineering at Instituto Superior de Engenharia de Lisboa (ISEL). This thesis was developed under the guidance of the following professors: Tiago Dias and André Lourenço.

## Table of contents
- [Overview](#overview)
- [Proposed Approach](#proposed-approach)
    - [ESP-IDF](#esp-idf)
    - [Communication](#communication)
    - [Parsing Tool](#parsing-tool)
    - [OpenSearch](#opensearch)
 

# Overview

Informatic systems find themselves in constant evolution, being by the addition of new functionalities or by bug fixing. To keep up with its growth, the introduction of an event or logging recording system is required to monitor and support the diagnosis of problems and failures in functional applications in test and production environments. 

This need also exists in IoT (Internet of Things) solutions and good decisions must be made to guarantee an effective logging process that doesn’t impact the normal functioning of the system. On the other hand, the use of a Cloud environment has been shown beneficial for both external processing made on logs and the respective visualization of information in dashboards.

CardioWheel is an IoT solution created by CardioID that monitors the overall state of its user during driving activity and requires the development of a logging system to track and observe data flow in the application. The occurrence of sensitive user information demands a carefully planned design of the different components in the logging system.

The work carried out within the scope of this dissertation aims to investigate and propose a solution that complies with all the mentioned limitations in IoT systems to allow a dynamic monitoring process of CardioWheel.

On this repository I will focus on showing how to set up the environment prepared to receive logging data generated from the implementation proposed to the CardioWheel devices.

# Proposed Approach

In this section, I'll present the environment used to develop the embebbed logging library and the main elements composing the Cloud's implementation of the logging system: communication, parsing tool, search engine, and visualization tool. The technologies were instantiated on the same physical machine with a Linux operating system and an Ubuntu 20.04.3 LTS distribution. We utilized the following hardware components: two 4 GB DDR3 RAM sticks with 1600 MHz of speed; Intel(R) Pentium(R) CPU G3240 @ 3.10GHz processor; 500 GB Hard Drive.

## ESP-IDF

The application was developed with [Espressif IoT Development Framework Plugin](https://github.com/espressif/esp-idf) (version 4.4.4) and [Visual Studio Code](https://code.visualstudio.com) (version 1.85.0).

Please check [ESP-IDF docs](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for getting started instructions.

## Communication

In order to guarantee a secure and consistent data transmission we applied the SSH File Transfer Protocol (SFTP), which uses the Secure Shell (SSH) to allow authentication and security. The actual SFTP protocol is applied on the IoT device and it is only necessary to open an SSH endpoint to where the device can open a connection and send data. To do this we utilize the [OpenSSH Tool](https://www.openssh.com). With that said, the steps to correctly prepare the communication between the device and the cloud environment are:
1. Install OpenSSH: sudo apt install openssh-server;
2. Generate an SSH key: ssh-keygen -t rsa. In my case, I chose the generate the public/private key with the RSA algorithm but there are a couple of other options available in order to customize your key generation process. By default the public key is saved in the file ~/.ssh/id_rsa.pub, while ~/.ssh/id_rsa is the private key;
3. Copy the public key to the remote host: ssh-copy-id username@remotehost. The key will be appended to ~/.ssh/authorized_keys file;
4. You'll want to make sure the SSH configuration, located under /etc/ssh/sshd_config, is appropriate. For example, if you feel like the password authentication won't be used then you should avoid having the "PasswordAuthentication yes" line;
5. (Optional) If you're planning to use your personal router and systems to build and implement this, you should then open the SSH endpoint to the public network if you plan to have connections coming from outside the local network. By default the SSH port will be 22 on your Ubuntu system, therefore you'll want to port forward the port 22 to a port of your choice in your router. Keep in mind that the IP won't be static, so you'll need to reconfigure the devices from time to time;
6. (Optional) If you intend to connect to the SSH tunnel via Windows or Linux, you should then install [Bitvise SSH Client](https://www.bitvise.com/ssh-client-download). You can then use the client key manager to import the private key generated previously. To log into the SSH tunnel you should: add the IP (local one if the connection is coming from your local network or public if not), port (22 by default if the connection is coming from your local network or the port forwarded configured if not), username configured on the ubuntu server, public key initial method and the imported client key;
7. Make sure the target directory of the IoT devices exists in the user directory.

## Parsing Tool

The parsing tool is the software component used to analyze and interpret the overall structure of the data with a specific syntax in mind. The tool introduced to apply this behavior was [Logstash](https://www.elastic.co/guide/en/logstash/current/introduction.html). To prepare the tool in accordance to the rest of the solution are:
1. Manually download the 8.6.1 version [here](https://www.elastic.co/downloads/past-releases/logstash-8-6-1) (LINUX X86_64 file);
2. Extract it to a directory of your choice in your Ubuntu machine;
3. To initialize the Logstash instance, you'll need to go to the bin directory inside the extracted folder and execute the "./logstash -f logstash.conf" command, in order to execute the instance with the configuration described below.

The configuration file is located inside the extracted folder under the name "logstash.conf". You'll want to open the file and add the following configuration information:
```
input {
	file {
		path => "/home/ganilha/kibana/cardioid*"
		mode => "tail"
	}
}

filter {
  	grok {
		match => { "message" => [
			"\[%{TIMESTAMP_ISO8601:timestamp}\] %{NUMBER:id} %{WORD:log_level} \(%{NUMBER:pid}\) %{DATA:context}: %{GREEDYDATA:content}",
			"%{NUMBER:id} %{WORD:log_level} \(%{NUMBER:pid}\) %{DATA:context}: %{GREEDYDATA:content}"
			]
		}
  	}

	if [content] =~ /.+=(\d+|\w+)/ {
		# Split the "content" field into an array based on the delimiter " "
  		split {
    			field => "content"
    			terminator => " "
  		}

		# Use a grok filter to capture multiple occurrences of "parameter=value" or "status=success" pattern for each element in the "content" array
  		grok {
    			match => {
      				"content" => [
        				"%{DATA:parameter}=%{NUMBER:value}",
					"%{DATA:parameter}=%{WORD:description}"
      				]
    			}
  		}
	}

	date {
    		match => ["timestamp", "YYYY-MM-dd HH:mm:ss"]
    		target => "@timestamp"
  	}
}

output {
	opensearch {
		hosts => ["https://127.0.0.1:9200"]
		index => "indexforlogstash"
		user => "admin"
		password => "admin"
		ssl => true
		ssl_certificate_verification => false
	}
}
```

I'll explain a couple of the main commands:
1. path => "/home/ganilha/kibana/cardioid*", this line tells the tool to analyze files under the "/home/ganilha/kibana" directory with a name format beginning with "cardioid" only.
2. hosts => ["https://127.0.0.1:9200"], this line defines the target output. The output specified refers to the tool explained in the next section, OpenSearch. As previously mentioned, the OpenSearch was executed on the same machine as the Logstash instance. By default, it uses the 9200 port.
3. index => "indexforlogstash", this line refers to the name of the cluster of data to where the data parsed belongs. It is later used to query and interpret the information in OpenSearch.
4. user => "admin", admin username used in OpenSearch.
5. password => "admin", admin password used in OpenSearch.
   
## OpenSearch

OpenSearch implements both the search engine [OpenSearch](https://opensearch.org/docs/latest/) and the visualization tool [OpenSearch Dashboards](https://opensearch.org/docs/latest/dashboards/index/). Here are the steps to prepare and configure the instances in accordance with the rest of the technologies:
1. Install [Docker](https://www.docker.com);
2. Create a new directory with the following docker-compose.yml file;
```
version: '3'
services:
  opensearch-node1:
    image: opensearchproject/opensearch:latest
    container_name: opensearch-node1
    environment:
      - cluster.name=opensearch-cluster
      - node.name=opensearch-node1
      - discovery.seed_hosts=opensearch-node1
      - cluster.initial_cluster_manager_nodes=opensearch-node1
      - bootstrap.memory_lock=true # along with the memlock settings below, disables swapping
      - "OPENSEARCH_JAVA_OPTS=-Xms512m -Xmx512m" # minimum and maximum Java heap size, recommend setting both to 50% of system RAM
    ulimits:
      memlock:
        soft: -1
        hard: -1
      nofile:
        soft: 65536 # maximum number of open files for the OpenSearch user, set to at least 65536 on modern systems
        hard: 65536
    volumes:
      - opensearch-data1:/usr/share/opensearch/data
    ports:
      - 9200:9200
      - 9600:9600 # required for Performance Analyzer
    networks:
      - opensearch-net
  opensearch-dashboards:
    image: opensearchproject/opensearch-dashboards:latest
    container_name: opensearch-dashboards
    ports:
      - 5601:5601
    expose:
      - "5601"
    environment:
      OPENSEARCH_HOSTS: '["https://opensearch-node1:9200"]'
    networks:
      - opensearch-net

volumes:
  opensearch-data1:

networks:
  opensearch-net:
```
3. The yml provided will prepare a standalone instance of the OpenSearch platform. In order to initialize it you'll want to execute the "docker-compose up" command inside the directory. Keep in mind that the OpenSearch images will first be downloaded from the repository if it's your first time executing the command;
4. After the execution a prompt will appear to connect to the 5601 port, which is the default port for OpenSearch Dashboards (username: admin, password: admin);
5. Once the platform is up and running you should then add the index mapping, which is OpenSearch's interpretation of the data sent by Logstash under the chosen index, in this example "indexforlogstash". The index list is available under OpenSearch Plugins -> Index Management -> Indices. You should create a new index, add the correct name, and add the mapping below;
```
{
  "properties": {
    "@timestamp": {
      "type": "date"
    },
    "@timestampdevice": {
      "type": "date"
    },
    "@version": {
      "fields": {
        "keyword": {
          "type": "keyword",
          "ignore_above": 256
        }
      },
      "type": "text"
    },
    "content": {
      "type": "keyword"
    },
    "context": {
      "type": "keyword"
    },
    "description": {
      "type": "keyword"
    },
    "event": {
      "type": "object",
      "properties": {
        "original": {
          "fields": {
            "keyword": {
              "type": "keyword",
              "ignore_above": 256
            }
          },
          "type": "text"
        }
      }
    },
    "host": {
      "type": "object",
      "properties": {
        "name": {
          "fields": {
            "keyword": {
              "type": "keyword",
              "ignore_above": 256
            }
          },
          "type": "text"
        }
      }
    },
    "id": {
      "type": "integer"
    },
    "log": {
      "type": "object",
      "properties": {
        "file": {
          "type": "object",
          "properties": {
            "path": {
              "fields": {
                "keyword": {
                  "type": "keyword",
                  "ignore_above": 256
                }
              },
              "type": "text"
            }
          }
        }
      }
    },
    "log_level": {
      "type": "keyword"
    },
    "message": {
      "type": "keyword"
    },
    "parameter": {
      "type": "keyword"
    },
    "parsed_timestamp": {
      "fields": {
        "keyword": {
          "type": "keyword",
          "ignore_above": 256
        }
      },
      "type": "text"
    },
    "pid": {
      "type": "integer"
    },
    "tags": {
      "type": "keyword"
    },
    "timestamp": {
      "fields": {
        "keyword": {
          "type": "keyword",
          "ignore_above": 256
        }
      },
      "type": "text"
    },
    "timestamps": {
      "fields": {
        "keyword": {
          "type": "keyword",
          "ignore_above": 256
        }
      },
      "type": "text"
    },
    "value": {
      "type": "integer"
    }
  }
}
```
