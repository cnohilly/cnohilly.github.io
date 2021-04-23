package code;


public class Driver {

	public Driver() {
		new StatTracker();
	}
	
	public static void main(java.lang.String[] args) throws Exception {
		new Driver();
		/*
		MySQLAccess dao = new MySQLAccess();
		dao.readDataBase();
		dao.getGametypeStats("Team");
		dao.getGametypeStats("Final");
		*/
	}
}
