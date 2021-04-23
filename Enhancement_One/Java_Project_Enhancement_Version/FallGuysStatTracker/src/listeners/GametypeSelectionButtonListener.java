package listeners;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import code.*;
import mysql.first.MySQLAccess;

public class GametypeSelectionButtonListener implements ActionListener {
	
	private String _gametype;
	private GUI _gui;
	private MySQLAccess _mysql;
	
	public GametypeSelectionButtonListener(GUI gui, String gametype, MySQLAccess mysql){
		_gui = gui;
		_gametype = gametype;
		_mysql = mysql;
	}
	public void actionPerformed(ActionEvent e) {
		_gui.setGametypeDisplayed(_gametype);
		try {
			_mysql.updateDisplayedMaps(_gui);
		} catch (Exception exc) {
		}
	}
	
}
