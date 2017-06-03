package harish.kuruganti.httpserver;

import android.app.Activity;
import android.os.Bundle;
import iki.elonen.SimpleWebServer;

public class Server extends Activity {

	TextView t;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
	}

}
