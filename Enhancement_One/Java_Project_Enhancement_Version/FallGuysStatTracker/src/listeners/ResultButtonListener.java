package listeners;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import code.*;
import mysql.first.MySQLAccess;

public class ResultButtonListener implements ActionListener {
	
	private int _result;
	private GUI _gui;
	private MySQLAccess _mysql;
	
	public ResultButtonListener(GUI gui, int result, MySQLAccess mysql){
		_gui = gui;
		_result = result;
		_mysql = mysql;
	}
	public void actionPerformed(ActionEvent e) {
		try {
			_mysql.endGame(_gui, _result);
		} catch (Exception exc) {
		}
		_gui.resetSelectionBoxes();
	}
	
}
