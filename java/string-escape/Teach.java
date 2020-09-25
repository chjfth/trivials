import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import org.apache.commons.text.StringEscapeUtils;

public class Teach {

	public static void main(String[] args) {

		try {
			String es0 = new String(Files.readAllBytes(Paths.get("homework0.txt")));

			String es1 = StringEscapeUtils.escapeJava(es0);
			String es2 = StringEscapeUtils.escapeJava(es1);
			System.out.println(es2);
		}
		catch (IOException e) {
		}
	}
}
