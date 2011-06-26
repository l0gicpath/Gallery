<?php

require_once(UTILS);

/**
 * Class to manage user accounts and sessions when they are connected to the server
 */
class ContactList {

	/**
	 * @var Mongo
	 */
	protected $dbCx;

	/**
	 * @var MongoCollection
	 */
	protected $dbListsColl;
	
	private $params;
	private $id;

	/**
	 * Instantiate a new ContactList object
	 * 
	 * @param string $id List id for object instance
	 */
	function __construct($id=null) {
		$this->dbCx = DBConnection::singleton()->getConnection();
		$this->dbListsColl = $this->dbCx->selectCollection(DB_NAME, 'lists');
		$this->params = array();
		
		if (!is_null($id)) {
			$q = array(
				'_id' => new MongoId($id)
			);
			$list = $this->dbListsColl->findOne($q);
			
			if (is_null($list)) {
				$this->id = null;
				$this->params = array();
			} else {
				$this->id = $id;
				$this->params = $list;
			}
		}
	}
	
	function __destruct() {
		$this->dbCx = null;
	}
	
	public function getOwnerId() {
		return (array_key_exists('owner', $this->params)) ? $this->params['owner'] : false;
	}
	
	public function addMember($memberData) {
		if (!array_key_exists('m', $this->params)) {
			$this->params['m'] = array();
		}
		
		// generate an id for that member that must be unique within the list
		$memberData['id'] = time() . mt_rand(100,999);
		
		$this->params['m'][] = $memberData;
		$this->dbListsColl->save($this->params);

		return $memberData['id'];
	}

	public function getMembers() {
		return (array_key_exists('m', $this->params)) ? $this->params['m'] : array();
	}
	
	public function getMember($id) {
		if (array_key_exists('m', $this->params)) {
			foreach($this->params['m'] as $m) {
				if ($m['id'] == $id) return $m;
			}
		}
		return false;
	}
	
	public function getMemberByEmail($email) {
		if (array_key_exists('m', $this->params) && $this->hasMemberEmail($email)) {
			foreach($this->params['m'] as $m) {
				if (in_array($email, $m['addresses']) ) {
					return $m;
				}
			}
		}
		return false;
	}
	
	public function hasMemberEmail($email) {
		$q = array(
			'_id' => new MongoId($this->id),
			'm.addresses' => $email
		);
		return (!is_null($this->dbListsColl->findOne($q, array('_id'))));
	}
}
?>
