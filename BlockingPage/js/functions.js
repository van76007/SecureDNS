function doConfirm()
{
	return confirm('Are you sure you want to proceed?');
}

function showHide(shID) 
{
	if (document.getElementById(shID)) 
	{
		if (document.getElementById(shID).style.display == 'none' || document.getElementById(shID).style.display == "") 
		{
			document.getElementById(shID).style.display = 'block';
		}
		else 
		{
			document.getElementById(shID).style.display = 'none';
		}
	}
}

function autoDisplay()
{
	if (document.getElementById('centered'))
	{
		if(window != window.top)
		{
			document.getElementById('centered').style.display = 'none';
			document.getElementById('forIframe').style.display = 'block';
			
			if (typeof window.innerWidth != 'undefined')
			{
				if(window.innerWidth > window.innerHeight)
				{
					document.getElementById('iframeLogo').className = "dynForIframeH";
				}
				else
				{
					document.getElementById('iframeLogo').className = "dynForIframeW";
				}
			}
			
			document.body.className = 'bodyIframe';
		}
		else
		{
			document.getElementById('centered').style.display = 'block';
			document.getElementById('forIframe').style.display = 'none';
		}
	}
}