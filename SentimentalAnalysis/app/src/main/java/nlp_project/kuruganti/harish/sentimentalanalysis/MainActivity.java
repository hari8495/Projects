package nlp_project.kuruganti.harish.sentimentalanalysis;

import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.ibm.watson.developer_cloud.alchemy.v1.AlchemyLanguage;
import com.ibm.watson.developer_cloud.alchemy.v1.model.DocumentSentiment;
import com.ibm.watson.developer_cloud.alchemy.v1.model.Sentiment;

import java.util.HashMap;
import java.util.Map;
import java.util.StringTokenizer;

public class MainActivity extends AppCompatActivity
{
    Button button;
    TextView txtview;
    String sentiment = "";
    EditText editText;

    private class AskWatsonTask extends AsyncTask<String,Void,String>
    {
        @Override
        protected String doInBackground(String... texttoanalyze)
        {
            try
            {
                AlchemyLanguage service = new AlchemyLanguage();
                service.setApiKey("94169b20db846c06cdba85504d99f8c70247af6c");

                Map<String,Object> params = new HashMap<String, Object>();
                params.put(AlchemyLanguage.TEXT, sentiment);
                DocumentSentiment sentimental_analysis = service.getSentiment(params).execute();

                System.out.println(sentimental_analysis);
                sentiment = sentimental_analysis.getSentiment().toString();
            }
            catch(Exception e)
            {
                e.printStackTrace();
            }

            runOnUiThread(new Runnable()
            {
                @Override
                public void run()
                {
                    txtview.setText(sentiment);
                }
            });
            return sentiment;
        }

    }


    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        button = (Button)findViewById(R.id.button);
        txtview = (TextView)findViewById(R.id.textview);
        editText = (EditText)findViewById(R.id.editText);
        button.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                sentiment = ""+ editText.getText();
                AskWatsonTask task = new AskWatsonTask();
                task.execute(new String[]{});
            }
        });

    }
}
