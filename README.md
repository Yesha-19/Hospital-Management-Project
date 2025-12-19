
# Hospital Management Project

<h2>Introduction</h2>
<p>The primary objective of this project is to apply and demonstrate fundamental <b>Object-Oriented Programming (OOP)</b> principles to solve a real-world problem.</p>

<hr>

<h2>Key Features</h2>
<ul>
    <li><b>Role Management:</b> Differentiates between Doctors and Patients with distinct attributes and functionalities.</li>
    <li><b>Patient Specialization:</b> Handles two types of patients—Patient and EmergencyPatient—with different billing logic.</li>
    <li><b>Dynamic Record Keeping:</b> Uses dynamic memory allocation to manage lists of doctors and patients, allowing the system to handle a variable number of records.</li>
    <li><b>Automated Doctor Recommendation:</b> Suggests the most cost-effective doctor based on the patient's illness and severity.</li>
    <li><b>Resource Management:</b> Tracks the availability of hospital rooms and assigns them to incoming patients.</li>
    <li><b>Data Persistence:</b> Saves the entire state of the system (doctors, patients, and room occupancy) to a file, allowing sessions to be resumed later.</li>
</ul>

<hr>

<h2>Concepts Used</h2>



<h3>1. Abstraction (Person Class)</h3>
<p>Abstraction is achieved through the <b>Person</b> class, which serves as an abstract base class.</p>
<ul>
    <li><b>Purpose:</b> It defines a common, generic interface for any entity that is a "person" within the system. It establishes a contract that all derived classes must follow.</li>
    <li><b>Implementation:</b>
        <ul>
            <li>It contains a common attribute, <b>name</b>.</li>
            <li>It declares <b>pure virtual functions</b> (<code>display()</code>, <code>save()</code>, <code>load()</code>), which forces any concrete subclass to provide its own implementation for these actions.</li>
            <li>An object of type Person can never be instantiated on its own.</li>
        </ul>
    </li>
</ul>

<h3>2. Inheritance (Doctor, Patient, EmergencyPatient Classes)</h3>
<p>Inheritance is used to create a logical "is-a" relationship between classes, promoting code reuse and extensibility.</p>
<ul>
    <li><b>Single-Level Inheritance:</b> The Doctor and Patient classes inherit publicly from the Person class. They inherit the name attribute and the obligation to implement the pure virtual functions defined in Person.</li>
    <li><b>Multi-Level Inheritance:</b> The EmergencyPatient class inherits from the Patient class. This allows EmergencyPatient to reuse all the properties and methods of a Patient while adding its own specialized behavior (a modified billing calculation).</li>
</ul>

<h3>3. Polymorphism (display() and calculateBill() Methods)</h3>
<p>Polymorphism allows objects of different classes to be treated as objects of a common parent class. The system leverages this powerfully through virtual functions.</p>
<ul>
    <li><b>Implementation:</b> The patients vector in the Hospital class is of type <code>vector&lt;Patient*&gt;</code>. It can hold pointers to both Patient objects and EmergencyPatient objects.</li>
    <li><b>Runtime Behavior:</b>
        <ul>
            <li>When <code>patient->display()</code> is called on an element in the vector, C++ determines at runtime whether to execute the <code>display()</code> method from the Patient class or the overridden version from the EmergencyPatient class.</li>
            <li>Similarly, <code>patient->calculateBill()</code> invokes the correct billing logic based on the actual object type, automatically applying the 1.5x multiplier for emergency cases.</li>
        </ul>
    </li>
</ul>

<h3>4. Encapsulation</h3>
<p>Encapsulation is the bundling of data (attributes) and the methods that operate on that data into a single unit (a class).</p>
<ul>
    <li><b>Implementation:</b> In all classes (Doctor, Patient, Hospital), data members are declared <b>private</b> or <b>protected</b>. This prevents direct, uncontrolled access from outside the class.</li>
    <li><b>Data Integrity:</b> Public methods (e.g., <code>getPatientCount()</code>, <code>setPatientCount()</code>) are provided to allow controlled access to the object's data, ensuring data integrity. The main logic for managing the hospital is encapsulated within the Hospital class.</li>
</ul>
