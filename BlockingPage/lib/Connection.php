<?php

	class Connection
	{
        var $db_connection = null;        // Database connection string
        var $db_server = null;            // Database server
        var $db_database = null;          // The database being connected to
        var $db_username = null;          // The database username
        var $db_password = null;          // The database password
        var $CONNECTED = false;           // Determines if connection is established
		var $test = "";


		function __construct($server, $database, $username, $password)
		{
			$this->db_server = $server;
            $this->db_database = $database;
            $this->db_username = $username;
            $this->db_password = $password;
		}
        
        public function Open()
        {
            if (!$this->CONNECTED)
            {
                try
                {
                    $this->db_connection = mysql_connect($this->db_server, $this->db_username, $this->db_password);
                    mysql_select_db($this->db_database);
					
                    if (!$this->db_connection)
                    {
                        throw new Exception('MySQL Connection Database Error: ' . mysql_error());
                    }
                    else
                    {
                        $this->CONNECTED = true;
                    }
                }
                catch (Exception $e)
                {
                    echo $e->GetMessage();
                }
            }
            else
            {
                return "Error: No connection has been established to the database. Cannot open connection.";
            }
        }

         public function Close()
         {
            if ($this->CONNECTED)
            {
                mysql_close($this->db_connection);
                $this->CONNECTED = false;
            }
            else
            {
                return "Error: No connection has been established to the database. Cannot close connection.";
            }
         }

      }
?>