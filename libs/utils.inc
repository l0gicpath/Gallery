<?php
/**
 * utils.inc
 *
 * Library of utility functions
 */


/**
 * Validate an email address.
 * Provide email address (raw input)
 * Returns true if the email address has the email address format and the domain exists.
 * @see http://www.linuxjournal.com/article/9585?page=0,3
 */
function validate_email_address($email) {
	if (! is_string($email)) {
		throw new InvalidArgumentException('email must be string');
	}
	$isValid = true;
	$atIndex = strrpos($email, "@");
	if (is_bool($atIndex) && !$atIndex) {
		return false;
	}
	$local = substr($email, 0, $atIndex);
	$localLen = strlen($local);
	if ($localLen < 1 || $localLen > 64) {
		return false;
	}
	$domain = substr($email, $atIndex+1);
	$domainLen = strlen($domain);
	if ($domainLen < 1 || $domainLen > 255) {
		return false;
	}
	if ($local[0] == '.' || $local[$localLen-1] == '.') {
		// local part starts or ends with '.'
		return false;
	}
	if (preg_match('/\\.\\./', $local)) {
		// local part has two consecutive dots
		return false;
	}
	if (!preg_match('/^[A-Za-z0-9\\-\\.]+$/', $domain)) {
		// character not valid in domain part
		return false;
	}
	if (preg_match('/\\.\\./', $domain)) {
		// domain part has two consecutive dots
		return false;
	}
	if (!preg_match('/^(\\\\.|[A-Za-z0-9!#%&`_=\\/$\'*+?^{}|~.-])+$/', str_replace("\\\\","",$local))) {
		// character not valid in local part unless 
		// local part is quoted
		if (!preg_match('/^"(\\\\"|[^"])+"$/', str_replace("\\\\","",$local))) {
			return false;
		}
	}
	return true;
}


/**
 * Generates a key to be used to validate sessions
 * @param int $length Specify desired key lenght, if null, random length between 32 and 64 characters will be generated
 * @param boolean $mixedCase If true, a miz of upper case and lower-case will be used in the key, if false, only lower case will be used
 * @return string
 */
function generateKey($length=null,$mixedCase=true) {
	
	if ($mixedCase) $chars = str_split('0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ');
	else $chars = str_split('0123456789abcdefghijklmnopqrstuvwxyz');
	
	$keyLen = (is_null($length)) ? mt_rand(32,64) : intval ($length);

	$key = '';
	while (strlen($key) < $keyLen) {
		$key .= $chars[mt_rand(0,(count($chars) - 1))];
	}
	
	return $key;
}



/**
 * Return the number of seconds corresponding to a time multiplier. A month is considered as 365/12 days
 *
 * @param string $timeScale Time multiplier: 's' seconds, 'i' minutes, 'h' hours, 'm' month, 'y' year 
 * @return integer
 */
function getTimeScaleSecondMultiplier($timeScale) {
	switch (strtolower($timeScale)) {
		case 's':
			$secMultiplier = 1;
			break;
		case 'i':
			$secMultiplier = 60;
			break;
		case 'h':
			$secMultiplier = 60 * 60;
			break;
		case 'd':
			$secMultiplier = 60 * 60 * 24;
			break;
		case 'w':
			$secMultiplier = 60 * 60 * 24 * 7;
			break;
		case 'm':
			$secMultiplier = 60 * 60 * 24 * (365/12);
			break;
		case 'y':
			$secMultiplier = 60 * 60 * 24 * 365;
			break;
		default:
			$secMultiplier = 1;
			break;
	}
	return $secMultiplier;
}

/**
 * Return an array with days, hours, minutes and seconds
 * for the duration given as a number of seconds
 *
 * @param int $seconds Duration in seconds
 * @param array $dict Provider of language string
 * @return array
 */
function formatDuration($seconds, $dict=null) {

	if (is_null($dict) || !is_a($dict,'Dictionary')) {
		$dict = new Dictionary(LOCALE_EN);
	}
	$getAbbr = create_function('$scale,$dict','return $dict->getString("time.abbr.".$scale);');
	
	$duration = array('s'=>$seconds);
	$duration['str']['s'] = $duration['s'] . $getAbbr('second',$dict);
	$duration['str']['sorted'] = array($duration['str']['s']);

	if ($duration['s'] >= 60) {

		$duration['i'] = floor($duration['s'] / 60);
		$duration['s'] = $duration['s'] % 60;
		$duration['str']['s'] = $duration['s'] . $getAbbr('second',$dict);
		$duration['str']['i'] = $duration['i'] . $getAbbr('minute',$dict);
		$duration['str']['sorted'] = array($duration['str']['i'],$duration['str']['s']);

		if ($duration['i'] >= 60) {

			$duration['h'] = floor($duration['i'] / 60);
			$duration['i'] = $duration['i'] % 60;
			$duration['str']['i'] = $duration['i'] . $getAbbr('minute',$dict);
			$duration['str']['h'] = $duration['h'] . $getAbbr('hour',$dict);
			$duration['str']['sorted'] = array($duration['str']['h'],$duration['str']['i'],$duration['str']['s']);

			if ($duration['h'] >= 24) {

				$duration['d'] = floor($duration['h'] / 24);
				$duration['h'] = $duration['h'] % 24;
				$duration['str']['h'] = $duration['h'] . $getAbbr('hour',$dict);
				$duration['str']['d'] = $duration['d'] . $getAbbr('day',$dict);
				$duration['str']['sorted'] = array($duration['str']['d'],$duration['str']['h'],$duration['str']['i'],$duration['str']['s']);
			}
		}
	}

	$long = array();
	$scales = array('s'=>'second','i'=>'minute','h'=>'hour','d'=>'day');
	foreach ($scales as $key => $scale) {
		if ((!array_key_exists($key, $duration)) || ($duration[$key] == 0)) continue;
		$long[] = ($duration[$key] >= 2) ? $duration[$key].' '.$dict->getString('time.fullPlural.'.$scale) : $duration[$key].' '.$dict->getString('time.full.'.$scale); 
	}
	$duration['str']['long'] = implode(', ',$long);
	
	return $duration;
}

function formatByteSize($numBytes, $decimal=null) {
	// format file size
	$sizeSuffix = array('B','KB','MB','GB');
	$size = $numBytes;
	$returnVal = array();
	do {
		if (!is_null($decimal)) {
			$multiplier = 10 * $decimal;
			$returnVal = array('value'=> (round($size*$multiplier) / $multiplier), 'scale'=> array_shift($sizeSuffix), 'str'=> '');
		}
		elseif ($size < 10) {
			// by default, for files with size less than 10, show 1 decimal points in formatted size
			$returnVal = array('value'=> (round($size*10) / 10), 'scale'=> array_shift($sizeSuffix), 'str'=> '');
		}
		else {
			$returnVal = array('value'=> (round($size)), 'scale'=> array_shift($sizeSuffix), 'str'=> '');
		}
		$size = ($size / 1024);
	} while ($size >= 1 && count($sizeSuffix) > 0);
	
	$returnVal['str'] = $returnVal['value'] . ' ' . $returnVal['scale'];
	return $returnVal;
};



/**
 * Takes an array of associative arrays ($set) and returns an array containing only the value of
 * $keyInElements for each of the elements on $set.
 * 
 * Example :
 * $set = array(array('a'=>1,'b'=>2), array('a'=>5,'b'=>8));
 * extractElementsKey($set,'b') would return array(2,8);
 *
 * @param array $set
 * @param string $keyInElements
 * @return array
 */
function extractElementsKey($set,$keyInElements) {
	$extractedData = array();
	foreach ($set as $element) {
		$extractedData[] = $element[$keyInElements];
	}
	return $extractedData;
}

?>
