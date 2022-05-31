/* Name : BATUL ZAMIN
 * Class : CS621 - OPERATING SYSTEMS
 * PROGRAM : Use each of the concurrency techniques: 
 * 1.	semaphores 
 * 2.	message passing
 * A parking lot with a capacity of N cars is on an island. 
 * A ferry with a capacity of M cars shuttles cars between the mainland and the island. 
 * The ferry may cross whenever there are no cars waiting to get on the ferry (even if it is not full) 
 * provided that there are cars waiting on the other side. 
 * The ferry may also cross whenever it is full or (in the case of crossing to the island) 
 * when the number of cars in the ferry would fill the parking lot. 
 * You are to avoid deadlock and starvation. You may assume that the system contains more than N cars 
 * and that N is greater than M. 
 */

import java.util.Scanner;
import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
	public static void main(String[] args) {
		Scanner scan = new Scanner(System.in);
		System.out.println("Welcome to the car shuttle service between Island and Mainland with our Ferry!");
		
		/*Input for ferry and island capacity*/
		System.out.println("Enter ferry capacity: ");
		int M = scan.nextInt();
		System.out.println("Enter Island capacity (greater than ferry capacity): ");
		int N = scan.nextInt();
		
		/*Number of threads = Number of cars*/
		int threadCount = ThreadLocalRandom.current().nextInt(N, N + M + 1);
		final ExecutorService exService = Executors.newFixedThreadPool(threadCount);
		System.out.println("\nThere are total " + threadCount + " cars on Mainland.\n");
	
		final IslandParkingLot island = new IslandParkingLot(N);
		final MessagePassing msg = new MessagePassing(M);
		
		/*Start executing ferry*/
		Ferry ferry = new Ferry(M, msg, island);
		exService.execute(ferry);
		
		/*Start executing car threads*/
		for(int i=0; i < threadCount; i++) {
			Car car = new Car(i, msg);
			exService.execute(car);
		}
		/*Stop all services*/
		exService.shutdown();
		scan.close();
	}
}
