<?php	
	class generic
	{
		var $vars;

		function __construct() {  }

		function get($var)
		{
			return $this->vars[$var];

		}
		
		function set($key,$value)
		{
			$this->vars[$key] = $value;
		}
	  
	  
		// loads a key => value array into the class
		function load($array)
		{	
		
			if(is_array($array))
			{
				foreach($array as $key=>$value)
				{
					$this->vars[$key] = $value;
				}
			}
		}

		
		// empties a specified setting or all of them
		function unload($vars = '')
		{
			if($vars)
			{
				if(is_array($vars))
				{
					foreach($vars as $var)
					{
						unset($this->vars[$var]);
					}
				}
				else
				{
					unset($this->vars[$vars]);
				}
			}
			else
			{
			  $this->vars = array();
			}
		}

		function get_all()
		{
			return $this->vars;
		}
	}
?>