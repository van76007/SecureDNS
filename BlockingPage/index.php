<?php
	include_once 'lib/BlockingContent.php';
	include_once 'lib/Settings.php';
	
	$host = $_SERVER['HTTP_HOST'];
	
	$hostIP = gethostbyname($host);
	$requestUri = !empty($_SERVER['REQUEST_URI']) ? $_SERVER['REQUEST_URI'] : null;
	$html = "";
	
	//if $hostIP == $host, resolution failed	
	//if($hostIP != $host)
	//{
		$blockedContent = new BlockedContent($host);
	
		$blockedIPhtml = $blockedContent->CheckBlockedIP($hostIP);
		$caiHtml = $blockedContent->CheckCAI();
		$malwareHtml = $blockedContent->CheckMalware();
		$rblsHtml = $blockedContent->CheckRBLs();
		
		//<span class='title'>
		$html .= "<p class='header'><highlight>" . $host . "</highlight> has been reported as unsafe.</p>
					<p class='description'>This host has been reported for containing threats to your computer that might reveal personal or financial information.</p>";
		$html .= "<a class='moreInfo' href=\"#\" onclick=\"showHide('details');\">more information</a>";
		$html .= "<br /><br /><div id='details'>" . $blockedIPhtml . $caiHtml . $malwareHtml . $rblsHtml . "</div>";
		
		if(!isset($_GET['subfp']))
		{
			session_start();
			$_SESSION['fp'] = $host;
			$_SESSION['returnUrl'] = urlencode("http://" . $host . $requestUri);
		
			$html .= "<br class='cleaner' /><a href='http://$host/fpreports.php' class='fp'>Report this as a mistake</a>";
		}
		
		if($blockedContent->ALLOWCONTINUE && empty($malwareHtml) && empty($caiHtml) && empty($blockedIphtml))
		{	
			$_SESSION['rbl'] = 1;
			
			if($hostIP != $host)
			{
				$url = !empty($requestUri) ? "http://$hostIP$requestUri" : "http://$hostIP";
				$html .= "<a href='" . $url . "' onClick='javascript:doConfirm();' class='continue'>Continue (not recommended)</a>";
			}
			else
			{
				$url = "#";
				$html .= "<a href='" . $url . "' onClick='javascript:doConfirm();' class='continue'>The domain you are trying to access no longer exists</a>";
			}
		}
		
		//$html .= "</span>";
	//}
	//else
	//{
		//$html .= "<span>#The domain you are trying to access no longer exists</span>";
	//}
	
	include 'inc/header.inc';
	echo $html;
	include 'inc/footer.inc';
?>