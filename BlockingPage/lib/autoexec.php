<?php
	function __autoload($class_name) {
		include 'lib/' . $class_name . '.php';
	}
?>