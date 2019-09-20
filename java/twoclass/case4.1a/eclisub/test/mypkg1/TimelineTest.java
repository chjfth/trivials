package mypkg1;

public class TimelineTest {

	public static void main(String[] args) {

		Timeline timeline = new Timeline();
		int expected = 41;

		timeline.setFetchCount(expected);
		int actual = timeline.getFetchCount();
		
		System.out.println("Fetch = "+actual);
	}
}
