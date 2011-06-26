<?php

require_once(UTILS);

/**
 * Class to manage user accounts and sessions when they are connected to the server
 */
class User {

	/**
	 * @var Mongo
	 */
	protected $dbCx;

	/**
	 * @var MongoCollection
	 */
	protected $dbUsersColl;
	
	private $params;
	private $id;
	private $ctxIoAccnt;

	/**
	 * Instantiate a new User object
	 * 
	 * @param string $id User id for object instance
	 */
	function __construct($id=null) {
		$this->dbCx = DBConnection::singleton()->getConnection();
		$this->dbUsersColl = $this->dbCx->selectCollection(DB_NAME, 'users');
		$this->params = array();
		
		if (!is_null($id)) {
			$q = array(
				'_id' => new MongoId($id)
			);
			$user = $this->dbUsersColl->findOne($q);
			
			if (is_null($user)) {
				$this->id = null;
				$this->params = array();
				$this->ctxIoAccnt = '';
			} else {
				$this->id = $id;
				$this->params = $user;
				$this->ctxIoAccnt = $user['ctxIO'][0]['email'];
			}
		}
	}
	
	function __destruct() {
		$this->dbCx = null;
	}

	public static function addUser($email, $password) {
		
		$coll = DBConnection::singleton()->getConnection()->selectCollection(DB_NAME, 'users');
		
		$salt = generateKey(mt_rand(16,24));
		$usrObj = array(
			'email' => $email,
			'salt' => $salt,
			'pwd' => hash_hmac('sha256', $salt.$password, SITE_KEY)
		);
		$coll->insert($usrObj, array('safe'=>true));
		
		return $usrObj['_id']->__toString();
	}
	
	public function authentify($email, $password) {
		
		// retrieve user matching given email
		$q = array(
			'email' => $email
		);
		$user = $this->dbUsersColl->findOne($q);

		// check if password matches
		if (is_null($user) || hash_hmac('sha256', $user['salt'].$password, SITE_KEY) != $user['pwd']) {
			return false;
		}

		$this->params = $user;
		$this->ctxIoAccnt = $user['ctxIO'][0]['email'];
		
		return $user['_id']->__toString();
	}
	
	public function getId() {
		return $this->id;
	}

	public function getEmail() {
		return (array_key_exists('email', $this->params)) ? $this->params['email'] : false;
	}
	
	public function setContextIoAccount($account) {
		foreach ($this->params['ctxIO'] as $key) {
			if ($key['email'] == $account) {
				$this->ctxIoAccnt = $account;
				return true;
			}
		}
		return false;
	}
	
	public function getContextIoAccount() {
		return $this->ctxIoAccnt;
	}
	
	public function getContextIoId($account = null) {
		if (!array_key_exists('ctxIO', $this->params)) return null;
		if (is_null($account)) $account = $this->ctxIoAccnt;
		foreach ($this->params['ctxIO'] as $key) {
			if ($key['email'] == $account) return $key['id'];
		}
		return null;
	}
		
	private function _loadAndCachePicture($fileId) {

		$cacheDir = PIC_CACHE."/".$this->getId();
		if (!file_exists($cacheDir)) {
			mkdir($cacheDir);
		}

		$ctxIO = new ContextIO(CONTEXTIO_CONSUMER_KEY, CONTEXTIO_CONSUMER_SECRET);
		$ctxIO->useAuthorizationHeaders(true);
		$picFile = $cacheDir."/".$fileId."_full";
		$ctxIO->downloadFile($this->getContextIoId(), array('fileid'=>$fileId), $picFile);
		
		// generate thumbnail of the picture
		$this->_makeThumbnail($picFile);
		
		return $picFile;
	}
	
	private function _makeThumbnail($filePath) {

		$src = imagecreatefromjpeg($filePath);
		$srcWidth = imagesx($src);
		$srcHeight = imagesy($src);
		
		$dstWidth = THUMBNAIL_MAX_WIDTH;
		$dstHeight = intval($srcHeight * ($dstWidth/$srcWidth));
		if ($dstHeight > THUMBNAIL_MAX_HEIGHT) {
			$dstHeight = THUMBNAIL_MAX_HEIGHT;
			$dstWidth = intval($srcWidth * ($dstHeight/$srcHeight));
		}
		
		$newImg = imagecreatetruecolor($dstWidth,$dstHeight);
		imagecopyresized($newImg,$src,0,0,0,0,$dstWidth,$dstHeight,$srcWidth,$srcHeight);
		imagejpeg($newImg, preg_replace("/_full$/", "_thumb", $filePath));
		
		return true;
	}

	public function fetchPictureList() {		
		// fetch files fron Context.IO for all emails of that member
		$ctxIO = new ContextIO(CONTEXTIO_CONSUMER_KEY, CONTEXTIO_CONSUMER_SECRET);
		$ctxIO->useAuthorizationHeaders(true);
		
		$r = $ctxIO->allFiles($this->getContextIoId(), array('limit' => 5000, 'since' => 1));
		if ($r === false) {
			throw new Exception($ctxIO->getLastResponse()->getRawResponse());
		}
		else {
			$files = $r->getData();
			$pictures = array();
			foreach ($files as $file) {
				foreach ($file['occurrences'] as $att) {
					if ($att['type'] == "image/jpeg") {
						$pictures[] = $att;
					}
				}
			}
			return $pictures;
		}
	}
	
	public function getPicture($fileId, $view = 'full') {
		if ($view != 'thumb') $view = 'full';
		$filePath = PIC_CACHE.'/'.$this->getId().'/'.$fileId.'_'.$view;
		if (!file_exists($filePath)) {
			$this->_loadAndCachePicture($fileId);
		}
		return $filePath;
	}
	
	public function getPictureThumbnail($fileId) {
		return $this->getPicture($fileId, 'thumb');
	}
}
?>
